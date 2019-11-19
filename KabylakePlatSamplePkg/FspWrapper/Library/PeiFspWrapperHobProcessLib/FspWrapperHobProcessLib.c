/** @file
  Provide FSP wrapper hob process related function.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2017 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification
**/

#include <PiPei.h>

#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/FspWrapperPlatformLib.h>
#include <Guid/GuidHobFspEas.h>
#include <Guid/MemoryTypeInformation.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/PcdDataBaseHobGuid.h>
#include <Guid/ZeroGuid.h>
#include <Ppi/Capsule.h>

#include <FspEas.h>
#include <FspmUpd.h>
#include <MemInfoHob.h>
#include <SmbiosCacheInfoHob.h>
#include <SmbiosProcessorInfoHob.h>
#include <Private/SiConfigHob.h>
// AMI_OVERRIDE_START - Need to check CSM support for FSP Wrapper Build
#include <Ppi/SiPolicy.h>
// AMI_OVERRIDE_END - Need to check CSM support for FSP Wrapper Build

extern EFI_GUID gFspWrapperReservedMemoryResourceHobGuid;

//
// Additional pages are used by DXE memory manager.
// It should be consistent between RetrieveRequiredMemorySize() and GetPeiMemSize()
//
#define PEI_ADDITIONAL_MEMORY_SIZE    (16 * EFI_PAGE_SIZE)

/**
  Get the mem size in memory type infromation table.

  @param[in] PeiServices  PEI Services table.

  @return the mem size in memory type infromation table.
**/
UINT64
GetMemorySizeInMemoryTypeInformation (
  IN EFI_PEI_SERVICES **PeiServices
  )
{
  EFI_STATUS                  Status;
  EFI_PEI_HOB_POINTERS        Hob;
  EFI_MEMORY_TYPE_INFORMATION *MemoryData;
  UINT8                       Index;
  UINTN                       TempPageNum;

  MemoryData = NULL;
  Status     = (*PeiServices)->GetHobList ((CONST EFI_PEI_SERVICES**)PeiServices, (VOID **) &Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION &&
      CompareGuid (&Hob.Guid->Name, &gEfiMemoryTypeInformationGuid)) {
      MemoryData = (EFI_MEMORY_TYPE_INFORMATION *) (Hob.Raw + sizeof (EFI_HOB_GENERIC_HEADER) + sizeof (EFI_GUID));
      break;
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  if (MemoryData == NULL) {
    return 0;
  }

  TempPageNum = 0;
  for (Index = 0; MemoryData[Index].Type != EfiMaxMemoryType; Index++) {
    //
    // Accumulate default memory size requirements
    //
    TempPageNum += MemoryData[Index].NumberOfPages;
  }

  return TempPageNum * EFI_PAGE_SIZE;
}

/**
  Get the mem size need to be reserved in PEI phase.

  @param[in] PeiServices  PEI Services table.

  @return the mem size need to be reserved in PEI phase.
**/
UINT64
RetrieveRequiredMemorySize (
  IN EFI_PEI_SERVICES **PeiServices
  )
{
  UINT64                      Size;

  Size = GetMemorySizeInMemoryTypeInformation (PeiServices);
  return Size + PEI_ADDITIONAL_MEMORY_SIZE;
}

/**
  Get the mem size need to be consumed and reserved in PEI phase.

  @param[in] PeiServices  PEI Services table.
  @param[in] BootMode     Current boot mode.

  @return the mem size need to be consumed and reserved in PEI phase.
**/
UINT64
GetPeiMemSize (
  IN EFI_PEI_SERVICES **PeiServices,
  IN UINT32           BootMode
  )
{
  UINT64                      Size;
  UINT64                      MinSize;

// AMI_OVERRIDE_START - AMI Recovery goes to DXE phase, so memory size is the same with other boot mode.
#if 0
  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    return PcdGet32 (PcdPeiRecoveryMinMemSize);
  }
#endif
// AMI_OVERRIDE_END - AMI Recovery goes to DXE phase, so memory size is the same with other boot mode.

  Size = GetMemorySizeInMemoryTypeInformation (PeiServices);

  if (BootMode == BOOT_ON_FLASH_UPDATE) {
    //
    // Maybe more size when in CapsuleUpdate phase ?
    //
    MinSize = PcdGet32 (PcdPeiMinMemSize);
  } else {
    MinSize = PcdGet32 (PcdPeiMinMemSize);
  }

  return MinSize + Size + PEI_ADDITIONAL_MEMORY_SIZE;
}

/**
  Post FSP-M HOB process for Memory Resource Descriptor.

  @param[in] FspHobList  Pointer to the HOB data structure produced by FSP.

  @return If platform process the FSP hob list successfully.
**/
EFI_STATUS
EFIAPI
PostFspmHobProcess (
  IN VOID                 *FspHobList
  )
{
  EFI_PEI_HOB_POINTERS Hob;
  UINT64               PeiMemSize;
  EFI_PHYSICAL_ADDRESS PeiMemBase;
//
// AdvancedFeaturesBegin
//
  UINT64               S3PeiMemSize;
  EFI_PHYSICAL_ADDRESS S3PeiMemBase;
//
// AdvancedFeaturesEnd
//
  EFI_STATUS           Status;
  EFI_BOOT_MODE        BootMode;
  EFI_PEI_CAPSULE_PPI  *Capsule;
  VOID                 *CapsuleBuffer;
  UINTN                CapsuleBufferLength;
  UINT64               RequiredMemSize;
  UINT64               ResourceLength;
  EFI_PEI_SERVICES     **PeiServices;
// AMI_OVERRIDE_START - In Capule mode, add EFI_RESOURCE_ATTRIBUTE_TESTED for CoreInitializeMemoryServices.
  EFI_RESOURCE_ATTRIBUTE_TYPE ResourceAttributeTested; 
// AMI_OVERRIDE_END - In Capule mode, add EFI_RESOURCE_ATTRIBUTE_TESTED for CoreInitializeMemoryServices.

  PeiServices = (EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();

  PeiServicesGetBootMode (&BootMode);

  PeiMemBase = 0;
  ResourceLength = 0;
  //
  // Parse the hob list from fsp
  // Report all the resource hob except MMIO and IO resource Hob's
  //
  if (BootMode != BOOT_ON_S3_RESUME) {
    PeiMemSize = GetPeiMemSize (PeiServices, BootMode);
    RequiredMemSize = RetrieveRequiredMemorySize (PeiServices);
    Hob.Raw = (UINT8 *)(UINTN)FspHobList;
    DEBUG((DEBUG_INFO, "FspHobList - 0x%x\n", FspHobList));
    
// AMI_OVERRIDE_START - In Capule mode, update BootMode for FSP.
{
    EFI_HOB_HANDOFF_INFO_TABLE    *HandOffHob;
      
    if (BootMode == BOOT_ON_FLASH_UPDATE) {    
      HandOffHob = Hob.HandoffInformationTable;
      HandOffHob->BootMode = BootMode;
    }
}
// AMI_OVERRIDE_END - In Capule mode, update BootMode for FSP.
    
    //
    // Find the largest available system Memory and use it for PeiMemory
    //
    while ((Hob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, Hob.Raw)) != NULL) {
      if ((Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY)
        && (Hob.ResourceDescriptor->PhysicalStart + Hob.ResourceDescriptor->ResourceLength <= BASE_4GB)
        && (Hob.ResourceDescriptor->PhysicalStart >= PeiMemBase)
        && (Hob.ResourceDescriptor->ResourceLength >= PeiMemSize)) {
           PeiMemBase = Hob.ResourceDescriptor->PhysicalStart + Hob.ResourceDescriptor->ResourceLength - PeiMemSize;
      }
      Hob.Raw = GET_NEXT_HOB (Hob);
    }
  }

  Hob.Raw = (UINT8 *)(UINTN)FspHobList;

  //
  // Skip the MMIO and IO reource map from the FSP Hob list
  //
  while ((Hob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, Hob.Raw)) != NULL) {
    if ((Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_IO) || (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_MEMORY_MAPPED_IO)) {
      Hob.Raw = GET_NEXT_HOB (Hob);
      continue;
    }
    ResourceLength = Hob.ResourceDescriptor->ResourceLength;
    DEBUG((DEBUG_INFO, "Resource start %lx resource length %lx resource type %d\n",Hob.ResourceDescriptor->PhysicalStart,Hob.ResourceDescriptor->ResourceLength,Hob.ResourceDescriptor->ResourceType));
    
// AMI_OVERRIDE_START - In Capule mode, add EFI_RESOURCE_ATTRIBUTE_TESTED for CoreInitializeMemoryServices.
    ResourceAttributeTested = 0;
    if ((Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY)
      && (Hob.ResourceDescriptor->PhysicalStart + Hob.ResourceDescriptor->ResourceLength <= BASE_4GB)) {        
      if (BootMode == BOOT_ON_FLASH_UPDATE) {
        ResourceAttributeTested = EFI_RESOURCE_ATTRIBUTE_TESTED;                
      }
    }
    DEBUG((DEBUG_INFO, "ResourceAttributeTested = %x.\n", ResourceAttributeTested));
// AMI_OVERRIDE_END - In Capule mode, add EFI_RESOURCE_ATTRIBUTE_TESTED for CoreInitializeMemoryServices.
    if (BootMode != BOOT_ON_S3_RESUME) {
      //
      // If the system memory found in FSP Hob is determined for PeiMemory. Split the Resource descriptor Hob
      //
      if ((Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY)
        && (Hob.ResourceDescriptor->PhysicalStart <= PeiMemBase)
        && (Hob.ResourceDescriptor->PhysicalStart + Hob.ResourceDescriptor->ResourceLength >= PeiMemBase + PeiMemSize)
        && (Hob.ResourceDescriptor->PhysicalStart + Hob.ResourceDescriptor->ResourceLength <= BASE_4GB)) {
        if ((CompareGuid (&Hob.ResourceDescriptor->Owner, &gZeroGuid))) {
          BuildResourceDescriptorHob (
          Hob.ResourceDescriptor->ResourceType,
// AMI_OVERRIDE_START - In Capule mode, add EFI_RESOURCE_ATTRIBUTE_TESTED for CoreInitializeMemoryServices.
//            Hob.ResourceDescriptor->ResourceAttribute,
		    Hob.ResourceDescriptor->ResourceAttribute | ResourceAttributeTested,
// AMI_OVERRIDE_END - In Capule mode, add EFI_RESOURCE_ATTRIBUTE_TESTED for CoreInitializeMemoryServices.
            PeiMemBase,
            PeiMemSize
            );
        } else {
          BuildResourceDescriptorWithOwnerHob (
            Hob.ResourceDescriptor->ResourceType,
// AMI_OVERRIDE_START - In Capule mode, add EFI_RESOURCE_ATTRIBUTE_TESTED for CoreInitializeMemoryServices.
//            Hob.ResourceDescriptor->ResourceAttribute,
            Hob.ResourceDescriptor->ResourceAttribute | ResourceAttributeTested,
// AMI_OVERRIDE_END - In Capule mode, add EFI_RESOURCE_ATTRIBUTE_TESTED for CoreInitializeMemoryServices.
            PeiMemBase,
            PeiMemSize,
            &Hob.ResourceDescriptor->Owner
            );
        }
        ResourceLength = (Hob.ResourceDescriptor->ResourceLength) -(PeiMemSize);
      }
    }

    //
    // Report the resource hob
    //
    if ((CompareGuid (&Hob.ResourceDescriptor->Owner, &gZeroGuid))) {
      BuildResourceDescriptorHob (
        Hob.ResourceDescriptor->ResourceType,
// AMI_OVERRIDE_START - In Capule mode, add EFI_RESOURCE_ATTRIBUTE_TESTED for CoreInitializeMemoryServices.
//        Hob.ResourceDescriptor->ResourceAttribute,    
        Hob.ResourceDescriptor->ResourceAttribute | ResourceAttributeTested,                                 
// AMI_OVERRIDE_END - In Capule mode, add EFI_RESOURCE_ATTRIBUTE_TESTED for CoreInitializeMemoryServices.
        Hob.ResourceDescriptor->PhysicalStart,
        ResourceLength
        );
    } else {
      BuildResourceDescriptorWithOwnerHob (
        Hob.ResourceDescriptor->ResourceType,
// AMI_OVERRIDE_START - In Capule mode, add EFI_RESOURCE_ATTRIBUTE_TESTED for CoreInitializeMemoryServices.
//        Hob.ResourceDescriptor->ResourceAttribute,    
        Hob.ResourceDescriptor->ResourceAttribute | ResourceAttributeTested,                                 
// AMI_OVERRIDE_END - In Capule mode, add EFI_RESOURCE_ATTRIBUTE_TESTED for CoreInitializeMemoryServices.
        Hob.ResourceDescriptor->PhysicalStart,
        ResourceLength,
        &Hob.ResourceDescriptor->Owner
        );
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  //
  // @todo: It is a W/A for SetMemorySpaceAttribute issue in PchSpi and PchReset drivers in FVMAIN of SSPT.
  //        We need to modify it instead of hard code here. Due to InstallEfiMemory is using hard code to
  //        describe memory resource, we have to hard code in here. Once InstallEfiMemory is merged, we should
  //        be able to remove this.
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE,
    0xFE000000,
    0x10000
    );
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE,
    0xFE010000,
    0x1000
    );

//
// AdvancedFeaturesBegin
//
  if (BootMode == BOOT_ON_S3_RESUME) {

    S3PeiMemBase = 0;
    S3PeiMemSize = 0;
    Status = GetS3MemoryInfo (&S3PeiMemSize, &S3PeiMemBase);
    ASSERT_EFI_ERROR (Status);
    DEBUG((DEBUG_INFO, "S3 memory %016XLh - %016LXh bytes\n", S3PeiMemBase, S3PeiMemSize));

    //
    // Make sure Stack and PeiMemory are not overlap
    //

    Status = PeiServicesInstallPeiMemory (
               S3PeiMemBase,
               S3PeiMemSize
               );
    ASSERT_EFI_ERROR (Status);
  } else {
//
// AdvancedFeaturesEnd
//
    //
    // Capsule mode
    //
    Capsule = NULL;
    CapsuleBuffer = NULL;
    CapsuleBufferLength = 0;
    if (BootMode == BOOT_ON_FLASH_UPDATE) {
      Status = PeiServicesLocatePpi (
                 &gEfiPeiCapsulePpiGuid,
                 0,
                 NULL,
                 (VOID **) &Capsule
                 );
      ASSERT_EFI_ERROR (Status);

      if (Status == EFI_SUCCESS) {
        Status = PeiServicesGetHobList ((void**)&Hob.Raw);
        while ((Hob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, Hob.Raw)) != NULL) {
          if ((Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY)
               && (Hob.ResourceDescriptor->PhysicalStart + Hob.ResourceDescriptor->ResourceLength <= BASE_4GB)
               && (Hob.ResourceDescriptor->PhysicalStart >= BASE_1MB)
               && (Hob.ResourceDescriptor->PhysicalStart != PeiMemBase)
               && (Hob.ResourceDescriptor->ResourceLength >= CapsuleBufferLength)) {
             CapsuleBufferLength = (UINTN)Hob.ResourceDescriptor->ResourceLength;
             CapsuleBuffer = (VOID*)(UINTN)Hob.ResourceDescriptor->PhysicalStart;

          }
          Hob.Raw = GET_NEXT_HOB (Hob);
        }

        //
        // Call the Capsule PPI Coalesce function to coalesce the capsule data.
        //
        Status = Capsule->Coalesce (PeiServices, &CapsuleBuffer, &CapsuleBufferLength);
      }
    }


    DEBUG((DEBUG_INFO, "FSP wrapper PeiMemBase      : 0x%08x\n", PeiMemBase));
    DEBUG((DEBUG_INFO, "FSP wrapper PeiMemSize      : 0x%08x\n", PeiMemSize));
    DEBUG((DEBUG_INFO, "FSP wrapper RequiredMemSize : 0x%08x\n", RequiredMemSize));


    //
    // Install efi memory
    //
    Status = PeiServicesInstallPeiMemory (
               PeiMemBase,
               PeiMemSize - RequiredMemSize
               );
    ASSERT_EFI_ERROR (Status);

    if (Capsule != NULL) {
      Status = Capsule->CreateState ((EFI_PEI_SERVICES **)PeiServices, CapsuleBuffer, CapsuleBufferLength);
    }
  } // AdvancedFeaturesContent


  //
  // Create a memory allocation HOB at fixed location for MP Services PPI AP wait loop.
  // Report memory region used by FSP.
  //
  BuildMemoryAllocationHob (
    PcdGet32 (PcdFspCpuPeiApWakeupBufferAddr),
    EFI_PAGE_SIZE,
    EfiBootServicesData
    );

  return EFI_SUCCESS;
}

/**
  Process FSP HOB list

  @param[in] FspHobList  Pointer to the HOB data structure produced by FSP.

**/
VOID
ProcessFspHobList (
  IN VOID                 *FspHobList
  )
{
  UINT8                 PhysicalAddressBits;
  UINT32                RegEax;
  EFI_PEI_HOB_POINTERS  FspHob;

  FspHob.Raw = FspHobList;

  AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
  if (RegEax >= 0x80000008) {
    AsmCpuid (0x80000008, &RegEax, NULL, NULL, NULL);
    PhysicalAddressBits = (UINT8) RegEax;
  } else {
    PhysicalAddressBits = 36;
  }

  ///
  /// Create a CPU hand-off information
  ///
  BuildCpuHob (PhysicalAddressBits, 16);

  //
  // Add all the HOBs from FSP binary to FSP wrapper
  //
  while (!END_OF_HOB_LIST (FspHob)) {
    if (FspHob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION) {
      //
      // Skip FSP binary creates PcdDataBaseHobGuid
      //
      if (!CompareGuid(&FspHob.Guid->Name, &gPcdDataBaseHobGuid)) {
// AMI_OVERRIDE_START - Need to check CSM support for FSP Wrapper Build
        if (CompareGuid(&FspHob.Guid->Name, &gSiConfigHobGuid)) {             
          EFI_STATUS                Status;
          SI_CONFIG                 *SiConfigHob;
          SI_POLICY_PPI             *SiPolicyPpi;
          SI_CONFIG                 *SiConfig;
          
          SiConfigHob = (SI_CONFIG *)GET_GUID_HOB_DATA (FspHob.Guid);
          DEBUG((DEBUG_INFO, "CsmFlag is %x from Fsp Hob.\n", SiConfigHob->CsmFlag));
          
          Status = PeiServicesLocatePpi(
                     &gSiPolicyPpiGuid,
                     0,
                     NULL,
                     (VOID **)&SiPolicyPpi
                     );
          if (!EFI_ERROR (Status)) {
              Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gSiConfigGuid, (VOID *) &SiConfig);
              ASSERT_EFI_ERROR (Status);
              DEBUG((DEBUG_INFO, "CsmFlag is %x from Fsp wrapper.\n", SiConfig->CsmFlag));              
              
              // Follow Fsp wrapper setting to override Fsp Hob.
              SiConfigHob->CsmFlag = SiConfig->CsmFlag;              
          }        
        }
// AMI_OVERRIDE_END - Need to check CSM support for FSP Wrapper Build
        BuildGuidDataHob (
          &FspHob.Guid->Name,
          GET_GUID_HOB_DATA(FspHob),
          GET_GUID_HOB_DATA_SIZE(FspHob)
        );
      }
    }
    FspHob.Raw = GET_NEXT_HOB (FspHob);
  }
}

/**
  Dump FSP SMBIOS memory info HOB

**/
VOID
DumpFspSmbiosMemoryInfoHob (
  VOID
  )
{
  MEMORY_INFO_DATA_HOB   *FspSmbiosMemoryInfo = NULL;
  UINT8                  ChannelIndex;
  UINT8                  DimmIndex;

  FspSmbiosMemoryInfo = (MEMORY_INFO_DATA_HOB *) GetFirstGuidHob (&gSiMemoryInfoDataGuid);
  if (FspSmbiosMemoryInfo != NULL) {
    DEBUG((DEBUG_INFO, "\nFspSmbiosMemoryInfo\n"));
    DEBUG((DEBUG_INFO, "  |-> Revision : %d\n", FspSmbiosMemoryInfo->Revision));
    DEBUG((DEBUG_INFO, "  |-> DataWidth : %d\n", FspSmbiosMemoryInfo->DataWidth));
    DEBUG((DEBUG_INFO, "  |-> MemoryType : %d\n", FspSmbiosMemoryInfo->MemoryType));
    DEBUG((DEBUG_INFO, "  |-> ConfiguredMemoryClockSpeed : %d\n", FspSmbiosMemoryInfo->ConfiguredMemoryClockSpeed));
    DEBUG((DEBUG_INFO, "  |-> ErrorCorrectionType : %d\n", FspSmbiosMemoryInfo->ErrorCorrectionType));
    DEBUG((DEBUG_INFO, "  |-> ChannelCount : %d\n", FspSmbiosMemoryInfo->Controller[0].ChannelCount));
    DEBUG((DEBUG_INFO, "  |-> ChannelInfo\n"));
    for (ChannelIndex = 0; ChannelIndex < MAX_CH; ChannelIndex++) {
      {
        CHANNEL_INFO  *ChannelInfo;

        ChannelInfo = &FspSmbiosMemoryInfo->Controller[0].ChannelInfo[ChannelIndex];
        DEBUG((DEBUG_INFO, "        |-> ChannelId : %d\n", ChannelInfo->ChannelId));
        DEBUG((DEBUG_INFO, "        |-> DimmCount : %d\n", ChannelInfo->DimmCount));
        DEBUG((DEBUG_INFO, "        |-> DimmInfo\n"));
        for (DimmIndex = 0; DimmIndex < MAX_DIMM; DimmIndex++) {
          {
            DIMM_INFO  *DimmInfo;

            DimmInfo = &FspSmbiosMemoryInfo->Controller[0].ChannelInfo[ChannelIndex].DimmInfo[DimmIndex];
            if (ChannelIndex < (MAX_CH - 1)) {
              DEBUG((DEBUG_INFO, "        |     |-> DimmId : %d\n", DimmInfo->DimmId));
              DEBUG((DEBUG_INFO, "        |     |-> SizeInMb : %d\n", DimmInfo->DimmCapacity));
            } else {
              DEBUG((DEBUG_INFO, "              |-> DimmId : %d\n", DimmInfo->DimmId));
              DEBUG((DEBUG_INFO, "              |-> SizeInMb : %d\n", DimmInfo->DimmCapacity));
            }
          }
        }
      }
    }
    DEBUG((DEBUG_INFO, "\n"));
  }
}


/**
  Dump FSP SMBIOS Processor Info HOB

**/
VOID
DumpFspSmbiosProcessorInfoHob (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS    Hob;
  EFI_STATUS        Status;
  SMBIOS_PROCESSOR_INFO     *FspSmbiosProcessorInfo   = NULL;

  Status = PeiServicesGetHobList (&Hob.Raw);
  if (!EFI_ERROR (Status)) {
      if (Hob.Raw != NULL) {
          if ((Hob.Raw = GetNextGuidHob (&gSmbiosProcessorInfoHobGuid, Hob.Raw)) != NULL) {
              FspSmbiosProcessorInfo = GET_GUID_HOB_DATA (Hob.Guid);
          }
       }

     if (FspSmbiosProcessorInfo != NULL) {
       DEBUG((DEBUG_INFO, "\n FspSmbiosProcessorInfo\n"));
       DEBUG((DEBUG_INFO, "  |-> Total Number Of Sockets   : 0x%x\n", FspSmbiosProcessorInfo->TotalNumberOfSockets));
       DEBUG((DEBUG_INFO, "  |-> Current Socket Number   : 0x%x\n", FspSmbiosProcessorInfo->CurrentSocketNumber));
       DEBUG((DEBUG_INFO, "  |-> ProcessorType : 0x%x\n", FspSmbiosProcessorInfo->ProcessorType));
       DEBUG((DEBUG_INFO, "  |-> ProcessorFamily : 0x%x\n", FspSmbiosProcessorInfo->ProcessorFamily));
       DEBUG((DEBUG_INFO, "  |-> ProcessorManufacturerStrIndex : 0x%x\n", FspSmbiosProcessorInfo->ProcessorManufacturerStrIndex));
       DEBUG((DEBUG_INFO, "  |-> ProcessorId     : 0x%x\n", FspSmbiosProcessorInfo->ProcessorId));
       DEBUG((DEBUG_INFO, "  |-> ProcessorVersionStrIndex : 0x%x\n", FspSmbiosProcessorInfo->ProcessorVersionStrIndex));
       DEBUG((DEBUG_INFO, "  |-> Voltage : 0x%x\n", FspSmbiosProcessorInfo->Voltage));
       DEBUG((DEBUG_INFO, "  |-> ExternalClockInMHz : 0x%x\n", FspSmbiosProcessorInfo->ExternalClockInMHz));
       DEBUG((DEBUG_INFO, "  |-> CurrentSpeedInMHz : 0x%x\n", FspSmbiosProcessorInfo->CurrentSpeedInMHz));
       DEBUG((DEBUG_INFO, "  |-> Status : 0x%x\n", FspSmbiosProcessorInfo->Status));
       DEBUG((DEBUG_INFO, "  |-> ProcessorUpgrade : 0x%x\n", FspSmbiosProcessorInfo->ProcessorUpgrade));
       DEBUG((DEBUG_INFO, "  |-> CoreCount : 0x%x\n", FspSmbiosProcessorInfo->CoreCount));
       DEBUG((DEBUG_INFO, "  |-> EnabledCoreCount : 0x%x\n", FspSmbiosProcessorInfo->EnabledCoreCount));
       DEBUG((DEBUG_INFO, "  |-> ThreadCount : 0x%x\n", FspSmbiosProcessorInfo->ThreadCount));
       DEBUG((DEBUG_INFO, "  |-> ProcessorCharacteristics : 0x%x\n", FspSmbiosProcessorInfo->ProcessorCharacteristics));
       DEBUG((DEBUG_INFO, "\n"));
      }
    }
}

/**
  Dump FSP SMBIOS Cache Info Hob

**/
VOID
DumpFspSmbiosCacheInfoHob (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS    Hob;
  EFI_STATUS        Status;
  SMBIOS_CACHE_INFO         *FspSmbiosCacheInfo   = NULL;

  Status = PeiServicesGetHobList (&Hob.Raw);
  if (!EFI_ERROR (Status)) {
      if (Hob.Raw != NULL) {
          if ((Hob.Raw = GetNextGuidHob (&gSmbiosCacheInfoHobGuid, Hob.Raw)) != NULL) {
              FspSmbiosCacheInfo = GET_GUID_HOB_DATA (Hob.Guid);
          }
       }

      if (FspSmbiosCacheInfo != NULL) {
         DEBUG((DEBUG_INFO, "\nFspSmbiosCacheInfo\n"));
         DEBUG((DEBUG_INFO, "  |-> ProcessorSocketNumber   : 0x%x\n", FspSmbiosCacheInfo->ProcessorSocketNumber));
         DEBUG((DEBUG_INFO, "  |-> NumberOfCacheLevels   : 0x%x\n", FspSmbiosCacheInfo->NumberOfCacheLevels));
         DEBUG((DEBUG_INFO, "  |-> SocketDesignationStrIndex : 0x%x\n", FspSmbiosCacheInfo->SocketDesignationStrIndex));
         DEBUG((DEBUG_INFO, "  |-> CacheConfiguration : 0x%x\n", FspSmbiosCacheInfo->CacheConfiguration));
         DEBUG((DEBUG_INFO, "  |-> MaxCacheSize  : 0x%x\n", FspSmbiosCacheInfo->MaxCacheSize));
         DEBUG((DEBUG_INFO, "  |-> InstalledSize : 0x%x\n", FspSmbiosCacheInfo->InstalledSize));
         DEBUG((DEBUG_INFO, "  |-> SupportedSramType : 0x%x\n", FspSmbiosCacheInfo->SupportedSramType));
         DEBUG((DEBUG_INFO, "  |-> CurrentSramType : 0x%x\n", FspSmbiosCacheInfo->CurrentSramType));
         DEBUG((DEBUG_INFO, "  |-> CacheSpeed : 0x%x\n", FspSmbiosCacheInfo->CacheSpeed));
         DEBUG((DEBUG_INFO, "  |-> ErrorCorrectionType : 0x%x\n", FspSmbiosCacheInfo->ErrorCorrectionType));
         DEBUG((DEBUG_INFO, "  |-> SystemCacheType : 0x%x\n", FspSmbiosCacheInfo->SystemCacheType));
         DEBUG((DEBUG_INFO, "  |-> Associativity : 0x%x\n", FspSmbiosCacheInfo->Associativity));
         DEBUG((DEBUG_INFO, "\n"));
      }
    }
}

/**
  Dump FSP HOB list

**/
VOID
DumpFspHobList (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS Hob;
  EFI_STATUS           Status;

  Status = PeiServicesGetHobList (&Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION) {
      DEBUG ((DEBUG_INFO, "FSP Extended    GUID HOB: {%g}\n", &(Hob.Guid->Name)));
    }
    if ((Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) &&
       (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_MEMORY_RESERVED)) {
      DEBUG ((DEBUG_INFO, "FSP Reserved Resource HOB: %016lX ~ %016lX\n", \
              Hob.ResourceDescriptor->PhysicalStart, Hob.ResourceDescriptor->PhysicalStart \
              + Hob.ResourceDescriptor->ResourceLength));
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }
}

/**
  Dump FSP memory resource

**/
VOID
DumpFspMemoryResource (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS Hob;
  EFI_STATUS           Status;

  Status = PeiServicesGetHobList (&Hob.Raw);
  DEBUG ((DEBUG_INFO, "\nFSP Memory Resource\n"));
  DEBUG ((DEBUG_INFO, "         Resource Range           Type    Attribute                   Owner\n"));
  DEBUG ((DEBUG_INFO, "================================= ==== ================ ====================================\n"));
  while ((Hob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, Hob.Raw)) != NULL) {
    if (!CompareGuid (&(Hob.ResourceDescriptor->Owner), &gZeroGuid)) {
      DEBUG ((DEBUG_INFO, "%016lx-%016lx %4x %016x %g\n",
              Hob.ResourceDescriptor->PhysicalStart,
              Hob.ResourceDescriptor->PhysicalStart + Hob.ResourceDescriptor->ResourceLength,
              Hob.ResourceDescriptor->ResourceType,
              Hob.ResourceDescriptor->ResourceAttribute,
              &(Hob.ResourceDescriptor->Owner)
              ));
    } else {
      DEBUG ((DEBUG_INFO, "%016lx-%016lx %4x %016x \n",
              Hob.ResourceDescriptor->PhysicalStart,
              Hob.ResourceDescriptor->PhysicalStart + Hob.ResourceDescriptor->ResourceLength,
              Hob.ResourceDescriptor->ResourceType,
              Hob.ResourceDescriptor->ResourceAttribute
              ));
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }
  DEBUG ((DEBUG_INFO, "\n"));
}

/**
  Dump FSP memory resource

**/
VOID
DumpFspGraphicsInfoHob (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS      Hob;
  EFI_STATUS                Status;
  EFI_PEI_GRAPHICS_INFO_HOB *FspGraphicsInfo = NULL;

  Status = PeiServicesGetHobList (&Hob.Raw);
  if (!EFI_ERROR (Status)) {
    if (Hob.Raw != NULL) {
      if ((Hob.Raw = GetNextGuidHob (&gEfiGraphicsInfoHobGuid, Hob.Raw)) != NULL) {
        FspGraphicsInfo = GET_GUID_HOB_DATA (Hob.Guid);
      }
    }
    if (FspGraphicsInfo != NULL) {
      DEBUG((DEBUG_INFO, "\nGraphicsInfo\n"));
      DEBUG((DEBUG_INFO, "  |-> FrameBufferBase : 0x%016lx\n", FspGraphicsInfo->FrameBufferBase));
      DEBUG((DEBUG_INFO, "  |-> FrameBufferSize : 0x%016lx\n", FspGraphicsInfo->FrameBufferSize));
      DEBUG((DEBUG_INFO, "  |-> GraphicsMode\n"));
      DEBUG((DEBUG_INFO, "    |-> Version              : 0x%08x\n", FspGraphicsInfo->GraphicsMode.Version));
      DEBUG((DEBUG_INFO, "    |-> HorizontalResolution : %d\n", FspGraphicsInfo->GraphicsMode.HorizontalResolution));
      DEBUG((DEBUG_INFO, "    |-> VerticalResolution   : %d\n", FspGraphicsInfo->GraphicsMode.VerticalResolution));
      DEBUG((DEBUG_INFO, "    |-> PixelFormat          : %d\n", FspGraphicsInfo->GraphicsMode.PixelFormat));
      DEBUG((DEBUG_INFO, "    |-> PixelInformation     : %d|%d|%d|%d\n",
        FspGraphicsInfo->GraphicsMode.PixelInformation.RedMask,
        FspGraphicsInfo->GraphicsMode.PixelInformation.GreenMask,
        FspGraphicsInfo->GraphicsMode.PixelInformation.BlueMask,
        FspGraphicsInfo->GraphicsMode.PixelInformation.ReservedMask
        ));
      DEBUG((DEBUG_INFO, "    |-> PixelsPerScanLine    : %d\n", FspGraphicsInfo->GraphicsMode.PixelsPerScanLine));
      DEBUG((DEBUG_INFO, "\n"));
    } else {
      DEBUG((DEBUG_INFO, "\nNo GraphicsInfo\n"));
    }
  }
}


/**
  Post FSP-S HOB process (not Memory Resource Descriptor).

  @param[in] FspHobList  Pointer to the HOB data structure produced by FSP.

  @return If platform process the FSP hob list successfully.
**/
EFI_STATUS
EFIAPI
PostFspsHobProcess (
  IN VOID                 *FspHobList
  )
{
// AMI_OVERRIDE_START - Need to check CSM support for FSP Wrapper Build
#if 0
  EFI_PEI_HOB_POINTERS  HobPtr;
  SI_CONFIG             *SiConfig;
#endif
// AMI_OVERRIDE_END - Need to check CSM support for FSP Wrapper Build  
  ProcessFspHobList (FspHobList);
// AMI_OVERRIDE_START - Need to check CSM support for FSP Wrapper Build  
#if 0
  //
  // Get Silicon Config data HOB and override the CsmFlag with wrapper PcdSiCsmEnable
  //
  HobPtr.Guid = GetFirstGuidHob (&gSiConfigHobGuid);
  SiConfig = (SI_CONFIG *)GET_GUID_HOB_DATA (HobPtr.Guid);
  if (PcdGetBool(PcdSiCsmEnable) == TRUE) {
    SiConfig->CsmFlag = 1;
  } else {
    SiConfig->CsmFlag = 0;
  }
#endif  
// AMI_OVERRIDE_END - Need to check CSM support for FSP Wrapper Build  

  DEBUG_CODE_BEGIN ();
    DumpFspSmbiosMemoryInfoHob ();
    DumpFspSmbiosProcessorInfoHob();
    DumpFspSmbiosCacheInfoHob();
    DumpFspGraphicsInfoHob ();
    DumpFspHobList ();
    DumpFspMemoryResource ();
  DEBUG_CODE_END ();

  return EFI_SUCCESS;
}
