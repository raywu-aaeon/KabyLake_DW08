/** @file
  Provides the basic interfaces to abstract a PCI Host Bridge Resource Allocation

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2017 Intel Corporation.

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

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/
#include "PciHostBridge.h"
#include "PciRootBridge.h"
#include <Protocol/SaPolicy.h>
#include <Protocol/SaGlobalNvsArea.h>
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>
#include <Library/HobLib.h>
#include <Private/SaConfigHob.h> // AdvancedFeaturesContent
// AMI_OVERRIDE_SA0011_START >>> 
#include <Token.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/NbChipsetGuid.h>
#include <Setup.h>
#include <NbSetupData.h>
#include <Setup/SaSetup.h>
// AMI_OVERRIDE_SA0012_START >>>
#include <AmiDxeLib.h>
#include <Library/AmiSdlLib.h>
// AMI_OVERRIDE_SA0012_END <<<

//-----------------------------------------------------------------------
// AMI_OVERRIDE_SA0013_START >>> OUT of Resources Generic support start..
//-----------------------------------------------------------------------
#define AMI_PCI_OUT_OF_RESOURCES_GUID   \
        { 0xf26dec72, 0x19cf, 0x4e26, 0xb1, 0xad, 0x0f,0x6a, 0xef, 0x4f, 0x1d, 0x47 }
#ifndef SIZE_63GB
#define BASE_63GB   0xFC0000000ULL
#endif
//-----------------------------------------------------------------------
// AMI_OVERRIDE_SA0013_END <<< OUT of Resources Generic support end.
//-----------------------------------------------------------------------

#define MAX_TOLUD_0_5G       20 
#define MAX_TOLUD_0_75G      21 
// AMI_OVERRIDE_SA0011_END <<<

///
/// Support 4G address space
///
#define HIGHEST_SUPPORTED_ADDRESS 0x8000000000L

///
/// Support above 4G MMIO address space
///
#ifndef BASE_128GB
#define BASE_128GB  0x2000000000ULL  //128GB
#endif
#ifndef SIZE_64GB
#define SIZE_64GB  0x1000000000ULL  //64GB
#endif

///
/// Hard code: Root Bridge Number within the host bridge
///            Root Bridge's attribute
///            Root Bridge's device path
///            Root Bridge's resource aperture
///
static UINTN                              RootBridgeNumber[1] = { 1 };
///
/// Hard code EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM  0 to support prefetchable memory allocation
///
// AMI_OVERRIDE_SA0014_START >>> for Token select PCIE COMBINE mode.
#if 0
static UINT64                             RootBridgeAttribute[1] = { EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM };
#else
static UINT64                             RootBridgeAttribute[1] = { AMI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM_MODE };
#endif
// AMI_OVERRIDE_SA0014_END <<< for Token select PCIE COMBINE mode.
static EFI_PCI_ROOT_BRIDGE_DEVICE_PATH    mEfiPciRootBridgeDevicePath[1] = {
  {
    {
      {
        ACPI_DEVICE_PATH,
        ACPI_DP,
        {
          (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)),
          (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)
        }
      },
      EISA_PNP_ID (0x0A03),
      0
    },
    {
      END_DEVICE_PATH_TYPE,
      END_ENTIRE_DEVICE_PATH_SUBTYPE,
      {
        END_DEVICE_PATH_LENGTH,
        0
      }
    }
  }
};

// AMI_OVERRIDE_SA0015_START >>> for Support Token change PCIEX_BASE_ADDRESS.
#if 0
static PCI_ROOT_BRIDGE_RESOURCE_APERTURE  mResAperture[1] = { { 0, 255, 0, 0xffffffff, 0, 1 << 16 } };
#else
// AMI_OVERRIDE_SA0023_START >>>
#ifdef AMI_PCIE_DYNAMIC_SUPPORT_FLAG 
static PCI_ROOT_BRIDGE_RESOURCE_APERTURE  mResAperture[1] = { { 0, ((UINT8)((0x10000000 >> 20) - 1)), 0, 0xffffffff, 0, 1 << 16 } };
#else
static PCI_ROOT_BRIDGE_RESOURCE_APERTURE  mResAperture[1] = { { 0, ((UINT8)((PCIEX_LENGTH >> 20) - 1)), 0, 0xffffffff, 0, 1 << 16 } };
#endif
// AMI_OVERRIDE_SA0023_END <<<
#endif
static NB_SETUP_DATA                   gNbSetupData = {0};
// AMI_OVERRIDE_SA0015_END <<<
static EFI_HANDLE                         mDriverImageHandle;


GLOBAL_REMOVE_IF_UNREFERENCED SA_POLICY_PROTOCOL                    *mSaPolicy;     // AdvancedFeaturesContent
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                                 mEnableAbove4GBMmioBiosAssignemnt;    // AdvancedFeaturesContent
GLOBAL_REMOVE_IF_UNREFERENCED SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL *mSaGlobalNvsAreaProtocol;
GLOBAL_REMOVE_IF_UNREFERENCED UINT64                                mMmio64Base;
GLOBAL_REMOVE_IF_UNREFERENCED UINT64                                mMmio64Length;
GLOBAL_REMOVE_IF_UNREFERENCED UINT32                                mMmio32Base;
GLOBAL_REMOVE_IF_UNREFERENCED UINT32                                mMmio32Length;
// AMI_OVERRIDE_SA0016_START >>> Fixed GenericSio use 0x0 ~ RES_IO_BASE issue.
static EFI_ALLOCATE_IO_SPACE          gAmiCoreAllocateIoSpace;
//-----------------------------------------------------------------------
// AMI_OVERRIDE_SA0013_START >>> OUT of Resources Generic support start..
//-----------------------------------------------------------------------
static EFI_GUID gAmiPciOutOfResVarGuid = AMI_PCI_OUT_OF_RESOURCES_GUID;

#pragma pack(push, 1)

//Large resource common header
typedef struct _ASLRF_L_HDR {
	union {
		UINT8	HDR;
		struct {
			UINT8		Name	: 7;
			UINT8		Type	: 1;
		};
	};
	UINT16			Length;
} ASLRF_L_HDR;

//Address Space Descriptor General Flags
typedef union _ASLRF_GFLAGS {
	UINT8		GFLAGS;
	struct {
		UINT8	_RPR		: 1;	//Bit 0=1 This Device only consume this resource
		UINT8	_DEC		: 1;	//if 1 Substractive Decoding 0 - Positive
		UINT8	_MIF		: 1;	//MIN address is Fixed
		UINT8	_MAF		: 1;	//MAX address is Fixed 
		UINT8	Reserved	: 4;
	};
} ASLRF_GFLAGS;

typedef union _ASLRF_TFLAGS_{
	UINT8		TFLAGS;
	struct {//for memory space descriptor type
		UINT8	_RW		:	1; //1=ReadWrite 0=ReadOnly
		UINT8	_MEM	:	2; //Memory Attributes see above
		UINT8	_MTP	: 	2; //Memory Type see above
		UINT8	_TTP 	: 	1; //Translation Type: 1=Translation 0=Static  
		UINT8	Reserved:	2;
	}MEM_FLAGS;
	struct {//For IO Space Descriptor
		UINT8	_RNG	: 	2; //Range see above
		UINT8	Reserved1:	2;
		UINT8	_TTP	:	1; //Translation Type: 1=Translation 0=Static  
		UINT8	_TRA	:	1; //Space Translation 1=Sparse; 0=Dense
		UINT8	Reserved2:	2;
	}IO_FLAGS;
} ASLRF_TFLAGS;

typedef struct _ASLR_QWORD_ASD {
	ASLRF_L_HDR		Hdr;
	UINT8			Type;
	ASLRF_GFLAGS	GFlags; //General Flags
	ASLRF_TFLAGS	TFlags;	//Type Specific Flags
	UINT64			_GRA;
	UINT64			_MIN;
	UINT64			_MAX;
	UINT64			_TRA;
	UINT64			_LEN;
} ASLR_QWORD_ASD;

typedef struct _AMI_OUT_OF_RES_VAR {
    ASLR_QWORD_ASD  Resource;
    UINTN           Count;    
} AMI_OUT_OF_RES_VAR;
#pragma pack(pop)
//*************************************************************************
//<AMI_PHDR_START>
//
// Name: AmiPciOutOfRes
//
// Description:
//  EFI_STATUS AmiPciOutOfRes(AMI_OUT_OF_RES_VAR *AmiOutOfResData, IN BOOLEAN Get) sets
// or retrieves the AMI_OUT_OF_RES_DATA from NVRAM depending upon the Get input.
//
// Input:
//  AMI_OUT_OF_RES_VAR *AmiOutOfResData
// If Get is TRUE, returns an address to the AMI_OUT_OF_RES_VAR.  If Get is FALSE,
// used to update the AMI_OUT_OF_RES_VAR data NVRAM variable.  User is responsible for
// allocating and managing this memory.
//
//  IN BOOLEAN Get
// If TRUE, causes function to return an address to the ISA DMA mask.
// If FALSE, causes function to use the user provided ISA DMA mask to update
// the NVRAM variable with its value.
//
// Output:
//  EFI_NOT_FOUND,           if the NVRAM variable is not found.
//  EFI_DEVICE_ERROR,        if the NVRAM variable could not be
//                          accessed due to a hardware error.
//  EFI_SECURITY_VIOLATION,  if the NVRAM variable could be not
//                          be accessed due to a authentication failure.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS AmiPciOutOfRes(AMI_OUT_OF_RES_VAR *AmiOutOfResData, BOOLEAN Get)
{
    AMI_OUT_OF_RES_VAR  orv;
    UINTN               sz=sizeof(AMI_OUT_OF_RES_VAR);
    EFI_STATUS          Status;
//-----------------------
    if(!Get && AmiOutOfResData==NULL) return EFI_INVALID_PARAMETER;

    if (Get) Status = gRT->GetVariable(L"AmiOutOfRes", &gAmiPciOutOfResVarGuid, NULL, &sz, &orv);
    else Status=gRT->SetVariable(L"AmiOutOfRes",&gAmiPciOutOfResVarGuid,EFI_VARIABLE_BOOTSERVICE_ACCESS, sz, AmiOutOfResData);

    if(Get && AmiOutOfResData!=NULL) *AmiOutOfResData=orv;
    
    return Status;
}

//-----------------------------------------------------------------------
// AMI_OVERRIDE_SA0013_END <<< OUT of Resources Generic support end.
//-----------------------------------------------------------------------

EFI_STATUS
NbCspOverrideCoreAllocateIoSpace (
  IN     EFI_GCD_ALLOCATE_TYPE  GcdAllocateType,
  IN     EFI_GCD_IO_TYPE        GcdIoType,
  IN     UINTN                  Alignment,
  IN     UINT64                 Length,
  IN OUT EFI_PHYSICAL_ADDRESS   *BaseAddress,
  IN     EFI_HANDLE             ImageHandle,
  IN     EFI_HANDLE             DeviceHandle OPTIONAL
  )
{
  EFI_STATUS                    Status;
  EFI_GCD_IO_SPACE_DESCRIPTOR   IoDescriptor;

  if ((GcdIoType == EfiGcdIoTypeIo) && (*BaseAddress < PcdGet16 (PcdPciReservedIobase))) {
    if (GcdAllocateType == EfiGcdAllocateAddress) {
      Status = gDS->GetIoSpaceDescriptor (*BaseAddress, &IoDescriptor);
      
      if (!EFI_ERROR(Status)) {
        if (IoDescriptor.GcdIoType == EfiGcdIoTypeNonExistent) {
          Status = gDS->AddIoSpace (
                          EfiGcdIoTypeIo,
                          *BaseAddress,
                          Length
                          );
//        ASSERT_EFI_ERROR (Status);
        }
      }
    }
  }

  return gAmiCoreAllocateIoSpace (
           GcdAllocateType,
           GcdIoType,
           Alignment,
           Length,
           BaseAddress,
           ImageHandle,
           DeviceHandle
           );
}
// AMI_OVERRIDE_SA0016_END <<< Fixed GenericSio use 0x0 ~ RES_IO_BASE issue.

// AMI_OVERRIDE_SA0011_START >>> EIP191481 Implement the proposal of AMI fxied MMIO.
EFI_STATUS 
HbCspAdjustMemoryMmioOverlap (
  PCI_ROOT_BRIDGE_INSTANCE    *RootBridgeInstance
  )
{
    EFI_STATUS                      Status;    
    EFI_GCD_MEMORY_SPACE_DESCRIPTOR *MemorySpaceMap;
    UINTN                           NumberOfDescriptors;
    PCI_RES_NODE                    res;
    BOOLEAN                         ResStatus;
    UINTN                           i;    
    EFI_PHYSICAL_ADDRESS            Highest4GMem    = 0;
    EFI_PHYSICAL_ADDRESS            LowestMMIO      = 0xffffffff;
    EFI_PHYSICAL_ADDRESS            LowestAllocMMIO = 0xffffffff;
    UINTN                           MemoryCeiling = 0; // Init to 0
    UINTN                           FixedMmioCeiling = 0x400; // Init to 0x400
    UINTN                           NewMemoryCeiling = 0xffffffff;
    UINTN                           DataSize = sizeof(UINT32);
    EFI_PHYSICAL_ADDRESS            NeededBottomMmio;

    GetNbSetupData (gRT, &gNbSetupData, FALSE);
    if(gNbSetupData.MaxTolud == 0) return EFI_SUCCESS;
    
    if((PcdGet8(PcdEfiGcdAllocateType) != EfiGcdAllocateMaxAddressSearchTopDown)) return EFI_SUCCESS;
    
    //------------------------------------
    Status = gRT->GetVariable ( 
                    L"MemCeil.",
                    &gAmiChipsetVariableGuid,
                    NULL,
                    &DataSize,
                    &MemoryCeiling );

    switch (gNbSetupData.MaxTolud) {
      case MAX_TOLUD_0_5G:
          FixedMmioCeiling = 0xE00;
          break;
      case MAX_TOLUD_0_75G:
          FixedMmioCeiling = 0xD00;
          break;
      case MAX_TOLUD_1G:
          FixedMmioCeiling = 0xC00;
          break;
      case MAX_TOLUD_1_25G:
          FixedMmioCeiling = 0xB00;
          break;
      case MAX_TOLUD_1_5G:
          FixedMmioCeiling = 0xA00;
          break;
      case MAX_TOLUD_1_75G:
          FixedMmioCeiling = 0x900;
          break;
      case MAX_TOLUD_2G:
          FixedMmioCeiling = 0x800;
          break;
      case MAX_TOLUD_2_25G:
          FixedMmioCeiling = 0x700;
          break;
      case MAX_TOLUD_2_5G:
          FixedMmioCeiling = 0x600;
          break;
      case MAX_TOLUD_2_75G:
          FixedMmioCeiling = 0x500;
          break;
      case MAX_TOLUD_3G:
          FixedMmioCeiling = 0x400;
          break;
      case MAX_TOLUD_3_25G:
          FixedMmioCeiling = 0x300;
          break;
      case MAX_TOLUD_3_5G:
          FixedMmioCeiling = 0x200;
          break;
    }
    
    //AMI_OVERRIDE_START >>>
        if((FixedMmioCeiling <= 0x200) && (gNbSetupData.MaxTolud != 0) && (PCIEX_BASE_ADDRESS == 0xE0000000)){
            FixedMmioCeiling += 0x100;
        }
    //AMI_OVERRIDE_END <<<
        
    FixedMmioCeiling = 0xffffffff - ((UINT32)LShiftU64(FixedMmioCeiling, 20)) + 1;
    if (FixedMmioCeiling > PCIEX_BASE_ADDRESS) FixedMmioCeiling = PCIEX_BASE_ADDRESS;
    
    
    DEBUG((-1, "GetVariable MemCeil Status = %r, MemoryCeiling = %8X.\n", Status, MemoryCeiling));
    
    // Memory sizing uses memory ceiling to set top of memory.

    Status = gDS->GetMemorySpaceMap (&NumberOfDescriptors, &MemorySpaceMap);
    ASSERT_EFI_ERROR(Status);

    // Find the lowest MMIO and lowest allocated MMIO in GCD.
    for (i = 0; i < NumberOfDescriptors; ++i) {
        EFI_GCD_MEMORY_SPACE_DESCRIPTOR *Descr = &MemorySpaceMap[i];
        EFI_PHYSICAL_ADDRESS Base = Descr->BaseAddress;
        
        //DEBUG((-1, "MemorySpace GcdMemoryType: %X \n", Descr->GcdMemoryType));
        //DEBUG((-1, "MemorySpace Base: %8X \n", Base));
        //DEBUG((-1, "MemorySpace Length: %8X \n", Descr->Length));

        // Find highest system below 4GB memory.
        // Treat any non MMIO as system memory. Not all system memory is
        // reported as system memory, such as SMM.
        if ((Descr->GcdMemoryType != EfiGcdMemoryTypeMemoryMappedIo) && (Base < LowestMMIO)) {
            EFI_PHYSICAL_ADDRESS EndMem = Base + Descr->Length - 1;
            if ((EndMem > Highest4GMem) && (EndMem <= 0xffffffff))
                Highest4GMem = EndMem;
        } // Find Lowest mmio above system memory.
        else if (Descr->GcdMemoryType == EfiGcdMemoryTypeMemoryMappedIo) {
            if (Base >= 0x100000) {
                if (Base < LowestMMIO)
                    LowestMMIO = Base;

                // If ImageHandle, then MMIO is allocated.
                // The LowestAllocMMIO address is affected by EFI_GCD_ALLOCATE_TYPE. 
                if ((Base < LowestAllocMMIO) && Descr->ImageHandle)
                    LowestAllocMMIO = Base;
            }
        }
    }

    gBS->FreePool(MemorySpaceMap);

    DEBUG((-1, "LowestMMIO : %8X \n", LowestMMIO));
    DEBUG((-1, "LowestAllocMMIO : %8X \n", LowestAllocMMIO));

    if (Highest4GMem + 1 != LowestMMIO) {
        DEBUG((-1, "PciHostCSHooks: "));
        DEBUG((-1, "System Memory and MMIO are not consequitive.\n"));
        DEBUG((-1, "Top of Below 4G Memory: %lX", Highest4GMem));
        DEBUG((-1, "Bottom of MMIO: %X\n", LowestMMIO));
    }
    
    if (LowestMMIO == LowestAllocMMIO) {
        DEBUG((-1, "There is a MMIO that is allocated by EfiGcdAllocateMaxAddressSearchBottomUp.\n"));
        DEBUG((-1, "It will make MemCeil increasing, so please change EFI_GCD_ALLOCATE_TYPE.\n"));
        ASSERT (FALSE);
        return EFI_SUCCESS;
    }

    // Find any MMIO that could not be allocated due to small of MMIO region.
    NeededBottomMmio = LowestAllocMMIO;    
    for(i = 0; i < TypeMax; ++i) {
        res = RootBridgeInstance->ResAllocNode[i];
        if ((res.Status != ResNone) && (res.Status != ResAllocated)) ResStatus = 1;
        else ResStatus = 0;
        
        if (((res.Type == TypeMem32) && ResStatus) || ((res.Type == TypePMem32) && ResStatus)) {            
            //Determine new memory ceiling variable needed to allocate this memory.
            NeededBottomMmio = NeededBottomMmio - res.Length;
            NeededBottomMmio &= ~(res.Alignment);
            //DEBUG((-1, "Res Base: %lX, Length: %lX\n", res.Base, res.Length));            
            //DEBUG((-1, "NeededBottomMmio : %8X \n", NeededBottomMmio));            
            if (NeededBottomMmio < NewMemoryCeiling) NewMemoryCeiling = (UINTN) NeededBottomMmio;
        }
    }
    
    if (NewMemoryCeiling < 0xffffffff) {    //Check if a NewMemory Ceiling is needed.
        // Adjust the granularity
        NewMemoryCeiling &= (~(TOP_LOW_MEM_GRANULARITY - 1));          
        // if NewMemoryCeiling > FixedMmioCeiling, will change to FixedMmioCeiling.
        if ( NewMemoryCeiling > FixedMmioCeiling)NewMemoryCeiling = FixedMmioCeiling;

        if (MemoryCeiling == NewMemoryCeiling) {
            // There is no hardware change in system configuration.
            return EFI_SUCCESS;
        }
    
        // Set memory ceiling variable.
        gRT->SetVariable(
               L"MemCeil.",
               &gAmiChipsetVariableGuid,
               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
               sizeof (UINT32),
               &NewMemoryCeiling
               );

        DEBUG((-1, "\nResetting System for NewMemoryCeiling : %X\n", NewMemoryCeiling));

#if (NV_SIMULATION != 1)
        // Don't reset system in case of NVRAM simulation
        gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
#endif
        // Control should not come here if NV_SIMULATION = 0.
    }

    return EFI_SUCCESS;
}
// AMI_OVERRIDE_SA0011_END <<< EIP191481 Implement the proposal of AMI fxied MMIO.

///
/// Implementation
///
/**
  A protocol callback which updates 64bits MMIO Base and Length in SA GNVS area

  @param[in] Event    - The triggered event.
  @param[in] Context  - Context for this event.

**/
VOID
UpdateSaGnvsForMmioResourceBaseLength (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS                    Status;

  Status = gBS->LocateProtocol (&gSaGlobalNvsAreaProtocolGuid, NULL, (VOID **) &mSaGlobalNvsAreaProtocol);
  if (Status != EFI_SUCCESS) {
    return;
  }
  gBS->CloseEvent (Event);

  DEBUG ((DEBUG_INFO, "[PciHostBridge] Update SA GNVS Area.\n"));
  mSaGlobalNvsAreaProtocol->Area->Mmio64Base                    = mMmio64Base;
  mSaGlobalNvsAreaProtocol->Area->Mmio64Length                  = mMmio64Length;
  mSaGlobalNvsAreaProtocol->Area->Mmio32Base                    = mMmio32Base;
  mSaGlobalNvsAreaProtocol->Area->Mmio32Length                  = mMmio32Length;
}

/**
  Entry point of PCI Host Bridge driver

  @param[in] ImageHandle -
  @param[in] SystemTable -

  @retval EFI_SUCCESS          - Driver Start OK
  @retval EFI_OUT_OF_RESOURCES - Fail to allocate required resource
  @retval EFI_DEVICE_ERROR     - Fail to install PCI_ROOT_BRIDGE_IO protocol.
**/
EFI_STATUS
EFIAPI
PciHostBridgeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                Status;
  UINTN                     Loop1;
  UINTN                     Loop2;
  PCI_HOST_BRIDGE_INSTANCE  *HostBridge;
  PCI_ROOT_BRIDGE_INSTANCE  *PrivateData;
  EFI_PHYSICAL_ADDRESS      PciBaseAddress;
  EFI_PHYSICAL_ADDRESS      RemapBase;
  EFI_PHYSICAL_ADDRESS      RemapLimit;
  EFI_PHYSICAL_ADDRESS      MeSegMask;
  EFI_PHYSICAL_ADDRESS      MeStolenSize;
  BOOLEAN                   MeStolenEnable;
  UINT32                    Tolud;
  UINT64                    Length;
  UINTN                     McD0BaseAddress;
  UINTN                     ResMemLimit1;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR PciMemorySpaceDescriptor;
  UINT64                    BaseAddress;
  UINT64                    Attributes;


  mDriverImageHandle = ImageHandle;

  mSaPolicy              = NULL;         // AdvancedFeaturesContent
  mEnableAbove4GBMmioBiosAssignemnt = 0; // AdvancedFeaturesContent
  mMmio64Base                       = 0;
  mMmio64Length                     = 0;
  mMmio32Base                       = 0;
  mMmio32Length                     = 0;
  ///
  /// This system has one Host Bridge (one Root Bridge in this Host Bridge)
  ///
  ///
  /// Create Host Bridge Device Handle
  ///
  for (Loop1 = 0; Loop1 < HOST_BRIDGE_NUMBER; Loop1++) {
    HostBridge = AllocatePool (sizeof (PCI_HOST_BRIDGE_INSTANCE));
    ASSERT (HostBridge != NULL);
    if (HostBridge == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    HostBridge->Signature         = PCI_HOST_BRIDGE_SIGNATURE;
    HostBridge->RootBridgeNumber  = RootBridgeNumber[Loop1];
    HostBridge->ResourceSubmited  = FALSE;
    HostBridge->CanRestarted      = TRUE;

    ///
    /// InitializeListHead (&HostBridge->Head);
    ///
    HostBridge->ResAlloc.NotifyPhase          = NotifyPhase;
    HostBridge->ResAlloc.GetNextRootBridge    = GetNextRootBridge;
    HostBridge->ResAlloc.GetAllocAttributes   = GetAttributes;
    HostBridge->ResAlloc.StartBusEnumeration  = StartBusEnumeration;
    HostBridge->ResAlloc.SetBusNumbers        = SetBusNumbers;
    HostBridge->ResAlloc.SubmitResources      = SubmitResources;
    HostBridge->ResAlloc.GetProposedResources = GetProposedResources;
    HostBridge->ResAlloc.PreprocessController = PreprocessController;
    HostBridge->HostBridgeHandle              = NULL;
    Status = gBS->InstallProtocolInterface (
                    &HostBridge->HostBridgeHandle,
                    &gEfiPciHostBridgeResourceAllocationProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &HostBridge->ResAlloc
                    );
    if (EFI_ERROR (Status)) {
      FreePool (HostBridge);
      return EFI_DEVICE_ERROR;
    }
    ///
    /// Create Root Bridge Device Handle in this Host Bridge
    ///
    InitializeListHead (&HostBridge->Head);
    for (Loop2 = 0; Loop2 < HostBridge->RootBridgeNumber; Loop2++) {
      PrivateData = AllocatePool (sizeof (PCI_ROOT_BRIDGE_INSTANCE));
      ASSERT (PrivateData != NULL);
      if (PrivateData == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      PrivateData->Signature  = PCI_ROOT_BRIDGE_SIGNATURE;
      PrivateData->DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) &mEfiPciRootBridgeDevicePath[Loop2];
// AMI_OVERRIDE_SA0023_START >>>
#ifdef AMI_PCIE_DYNAMIC_SUPPORT_FLAG
      mResAperture[Loop2].BusLimit = ((UINT8)((PCIEX_LENGTH >> 20) - 1));
#endif
// AMI_OVERRIDE_SA0023_END <<<
      RootBridgeConstructor (
        &PrivateData->Io,
        HostBridge->HostBridgeHandle,
        RootBridgeAttribute[Loop2],
        &mResAperture[Loop2]
        );
      PrivateData->Handle = NULL;
      Status = gBS->InstallMultipleProtocolInterfaces (
                      &PrivateData->Handle,
                      &gEfiDevicePathProtocolGuid,
                      PrivateData->DevicePath,
                      &gEfiPciRootBridgeIoProtocolGuid,
                      &PrivateData->Io,
                      NULL
                      );
      if (EFI_ERROR (Status)) {
        FreePool (PrivateData);
        return EFI_DEVICE_ERROR;
      }

      InsertTailList (&HostBridge->Head, &PrivateData->Link);
    }
  }
  ///
  /// Allocate 56 KB of I/O space [0x2000..0xFFFF]
  ///
  DEBUG ((DEBUG_INFO, " Allocating  IO space for PCI from 0x%X to 0x%X\n", PcdGet16 (PcdPciReservedIobase) ,PcdGet16 (PcdPciReservedIoLimit)));
  Status = gDS->AddIoSpace (
                  EfiGcdIoTypeIo,
                  PcdGet16 (PcdPciReservedIobase),
                  PcdGet16 (PcdPciReservedIoLimit) - PcdGet16 (PcdPciReservedIobase) + 1
                  );
  ASSERT_EFI_ERROR (Status);

// AMI_OVERRIDE_SA0016_START >>> Fixed GenericSio use 0x0 ~ RES_IO_BASE issue.
  gAmiCoreAllocateIoSpace = gDS->AllocateIoSpace;
  gDS->AllocateIoSpace = NbCspOverrideCoreAllocateIoSpace;
// AMI_OVERRIDE_SA0016_END <<< Fixed GenericSio use 0x0 ~ RES_IO_BASE issue.

  ///
  /// Allocate PCI memory space.
  ///
  ///
  /// Read memory map registers
  ///
  McD0BaseAddress = MmPciBase (SA_MC_BUS, 0, 0);
  RemapBase       = (MmioRead32 (McD0BaseAddress + R_SA_REMAPBASE) + LShiftU64(MmioRead32 (McD0BaseAddress + R_SA_REMAPBASE + 4), 32)) & B_SA_REMAPBASE_REMAPBASE_MASK;
  RemapLimit      = (MmioRead32 (McD0BaseAddress + R_SA_REMAPLIMIT) + LShiftU64(MmioRead32 (McD0BaseAddress + R_SA_REMAPLIMIT + 4), 32)) & B_SA_REMAPLIMIT_REMAPLMT_MASK;
  Tolud           = MmioRead32 (McD0BaseAddress + R_SA_TOLUD) & B_SA_TOLUD_TOLUD_MASK;
  PciBaseAddress  = Tolud;
  MeSegMask       = (MmioRead32(McD0BaseAddress + R_SA_MESEG_MASK) + LShiftU64(MmioRead32(McD0BaseAddress + R_SA_MESEG_MASK + 4), 32));
  MeStolenEnable  = (BOOLEAN) ((MeSegMask & B_SA_MESEG_MASK_ME_STLEN_EN_MASK) != 0);

  ///
  /// First check if memory remap is used
  ///
  if ((RemapBase > RemapLimit) && (MeStolenEnable)) {
    MeStolenSize = MeSegMask & B_SA_MESEG_MASK_MEMASK_MASK;
    if (MeStolenSize != 0) {
      MeStolenSize = HIGHEST_SUPPORTED_ADDRESS - MeStolenSize;
    }
    ///
    /// Remap is disabled -> PCI starts at TOLUD + ME Stolen size
    ///
    PciBaseAddress += MeStolenSize;
  }

  ResMemLimit1 = PcdGet32 (PcdPciReservedMemLimit);
  if (ResMemLimit1 == 0) {
    ResMemLimit1 = ((UINTN) MmPciBase (0,0,0));
  }

  Length = ResMemLimit1 - PciBaseAddress;

  if (Length != 0) {
    DEBUG ((DEBUG_INFO, " Allocating PCI space from 0x%X to 0x%X\n", (UINT32) PciBaseAddress, (UINT32)
            (PciBaseAddress + Length - 1)));
    Status = gDS->AddMemorySpace (
                    EfiGcdMemoryTypeMemoryMappedIo,
                    PciBaseAddress,
                    Length,
                    0
                    );
    ASSERT_EFI_ERROR (Status);
    mMmio32Base = (UINT32) PciBaseAddress;
    mMmio32Length = (UINT32) Length;
  }

  ///
  /// Reserve Pcie MMIO range
  ///
  DEBUG ((EFI_D_INFO, "Allocating PCIe MMIO space from 0x%X to 0x%X\n", PcdGet64 (PcdPciExpressBaseAddress), PcdGet64 (PcdPciExpressBaseAddress) + PcdGet32 (PcdPciExpressRegionLength) - 1));
  Status = gDS->AddMemorySpace (
                EfiGcdMemoryTypeMemoryMappedIo,
                PcdGet64 (PcdPciExpressBaseAddress),
                PcdGet32 (PcdPciExpressRegionLength),
                0
                );

  //
  //  Change PCI memory space attribute to Runtime Memory
  //
  BaseAddress = (EFI_PHYSICAL_ADDRESS)PcdGet64 (PcdPciExpressBaseAddress);
  Length      = PcdGet32 (PcdPciExpressRegionLength);
  Status      = gDS->GetMemorySpaceDescriptor (BaseAddress, &PciMemorySpaceDescriptor);
  ASSERT_EFI_ERROR (Status);

  Attributes  = PciMemorySpaceDescriptor.Attributes | EFI_MEMORY_RUNTIME;

  Status      = gDS->SetMemorySpaceAttributes (
                         BaseAddress,
                         Length,
                         Attributes
                         );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "Successfully changed memory attribute for PCIe\n"));

// AMI_OVERRIDE_SA0012_START >>>
  //Reserve CSP MMIO and IO reported in AMI_SDL_PCI_DATA.CspResCount trough GCD services.
  //If we here gSdlPciData must be initialized get
  DEBUG((DEBUG_INFO," HB: Reserve CSP Resources( ImageHandle=0x%X)\n", ImageHandle));
  DEBUG((DEBUG_INFO,"---------------------------------------------------------------------\n"));
  //Init Lib globals...
  InitAmiLib(ImageHandle,SystemTable);
  Status = AmiSdlInitBoardInfo();
  DEBUG((DEBUG_INFO," AmiSdlInitBoardInfo() = %r\n",Status));
  Status = AmiSdlReserveCspResources(ImageHandle);
  DEBUG((DEBUG_INFO,"---------------------------------------------------------------------\n"));
  DEBUG((DEBUG_INFO," HB: Reserve CSP Resources Status=%r\n",Status));
// AMI_OVERRIDE_SA0012_END <<<
  
  return EFI_SUCCESS;
}

/**
  Enter a certain phase of the PCI enumeration process

  @param[in] This  - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL instance
  @param[in] Phase - The phase during enumeration

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  Wrong phase parameter passed in.
  @retval EFI_NOT_READY          -  Resources have not been submitted yet.
**/
EFI_STATUS
EFIAPI
NotifyPhase (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE    Phase
  )
{
  PCI_HOST_BRIDGE_INSTANCE  *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE  *RootBridgeInstance;
  PCI_RESOURCE_TYPE         Index;
  LIST_ENTRY                *List;
  EFI_PHYSICAL_ADDRESS      BaseAddress;
  UINT64                    AddrLen;
  UINTN                     BitsOfAlignment;
  UINT64                    Alignment;
  EFI_STATUS                Status;
  EFI_STATUS                ReturnStatus;
  UINT64                    Length;    // AdvancedFeaturesContent
  EFI_PHYSICAL_ADDRESS      PciBaseAddress;    // AdvancedFeaturesContent
  BOOLEAN                   MmioHasBeenAllocated;
  EFI_EVENT                 SaGlobalNvsUpdateEvent;
  VOID                      *SaGlobalNvsInstalledRegistration;
  SA_CONFIG_HOB             *SaConfigHob;    // AdvancedFeaturesContent
  MISC_DXE_CONFIG           *MiscDxeConfig;  // AdvancedFeaturesContent

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  RootBridgeInstance = NULL;
  switch (Phase) {
    case EfiPciHostBridgeBeginEnumeration:
      if (HostBridgeInstance->CanRestarted) {
        ///
        /// Reset the Each Root Bridge
        ///
        List = HostBridgeInstance->Head.ForwardLink;
        while (List != &HostBridgeInstance->Head) {
          RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
          for (Index = TypeIo; Index < TypeMax; Index++) {
            RootBridgeInstance->ResAllocNode[Index].Type    = Index;
            RootBridgeInstance->ResAllocNode[Index].Base    = 0;
            RootBridgeInstance->ResAllocNode[Index].Length  = 0;
            RootBridgeInstance->ResAllocNode[Index].Status  = ResNone;
          }

          List = List->ForwardLink;
        }

        HostBridgeInstance->ResourceSubmited  = FALSE;
        HostBridgeInstance->CanRestarted      = TRUE;
#ifndef MINTREE_FLAG
        ///
        /// Retrieve SaPolicy and see if above 4GB MMIO BIOS assignment enabled
        ///
        if (mSaPolicy == NULL) {
          Status = gBS->LocateProtocol (&gSaPolicyProtocolGuid, NULL, (VOID **) &mSaPolicy);
          if (mSaPolicy != NULL) {
            Status = GetConfigBlock ((VOID *) mSaPolicy, &gMiscDxeConfigGuid, (VOID *)&MiscDxeConfig);
            ASSERT_EFI_ERROR (Status);
            if ((MiscDxeConfig != NULL) && (MiscDxeConfig->EnableAbove4GBMmio == 1)) {
              mEnableAbove4GBMmioBiosAssignemnt = 1;
            }
          }
          ///
          /// Enable Above 4GB MMIO when Aperture Size is 2GB or higher
          ///
          ///
          /// Get SaConfigHob HOB
          ///
          SaConfigHob = NULL;
          SaConfigHob = (SA_CONFIG_HOB *) GetFirstGuidHob (&gSaConfigHobGuid);
          if ((SaConfigHob != NULL) && (SaConfigHob->ApertureSize >= 15)) {
            mEnableAbove4GBMmioBiosAssignemnt = 1;
          }
          if (mEnableAbove4GBMmioBiosAssignemnt == 1) {
            ///
            /// Provide 64GB available above 4GB MMIO resource
            /// limited to use single variable MTRR to cover this above 4GB MMIO region.
            ///
            PciBaseAddress = BASE_128GB;
            Length         = SIZE_64GB;
            if (Length != 0) {
              DEBUG ((DEBUG_INFO, " Allocating PCI space from 0x%lX", (UINT64) PciBaseAddress));
              DEBUG ((DEBUG_INFO, " to 0x%lX\n", (UINT64) (PciBaseAddress + Length - 1)));
              Status = gDS->AddMemorySpace (
                              EfiGcdMemoryTypeMemoryMappedIo,
                              PciBaseAddress,
                              Length,
                              EFI_MEMORY_UC
                              );
              ASSERT_EFI_ERROR (Status);
              DEBUG ((DEBUG_INFO, " [HostBridge]Setting Attributes...\n"));
              Status = gDS->SetMemorySpaceAttributes (
                              PciBaseAddress,
                              Length,
                              EFI_MEMORY_UC
                              );
              ASSERT_EFI_ERROR (Status);
              if (Status == EFI_SUCCESS) {
                mEnableAbove4GBMmioBiosAssignemnt = 1;
                if (RootBridgeInstance != NULL) {
                  RootBridgeInstance->MemLimit = MAX_ADDRESS;
                }
                ///
                /// Update variables for above 4GB MMIO Base/Length.
                ///
                mMmio64Base   = PciBaseAddress;
                mMmio64Length = Length;
              } else {
                ///
                /// Something wrong when creating above 4GB resource -> disable the support.
                ///
                mEnableAbove4GBMmioBiosAssignemnt = 0;
              }
            }
          }
#endif //MINTREE_FLAG
          ///
          /// If SA Global NVS protocol not installed yet, register SA Global Nvs protocol callback event
          ///
          Status = gBS->LocateProtocol (&gSaGlobalNvsAreaProtocolGuid, NULL, (VOID **) &mSaGlobalNvsAreaProtocol);
          if (Status == EFI_NOT_FOUND) {
            Status = gBS->CreateEvent (
                            EVT_NOTIFY_SIGNAL,
                            TPL_CALLBACK,
                            (EFI_EVENT_NOTIFY) UpdateSaGnvsForMmioResourceBaseLength,
                            NULL,
                            &SaGlobalNvsUpdateEvent
                            );
            ASSERT_EFI_ERROR (Status);
            Status = gBS->RegisterProtocolNotify (
                            &gSaGlobalNvsAreaProtocolGuid,
                            SaGlobalNvsUpdateEvent,
                            &SaGlobalNvsInstalledRegistration
                            );
            ASSERT_EFI_ERROR (Status);
          } else {
            UpdateSaGnvsForMmioResourceBaseLength (NULL, NULL);
          }
        }  // AdvancedFeaturesContent
      } else {
        ///
        /// Can not restart
        ///
        return EFI_NOT_READY;
      }
      break;

    case EfiPciHostBridgeEndEnumeration:
      break;

    case EfiPciHostBridgeBeginBusAllocation:
      ///
      /// No specific action is required here, can perform any chipset specific programing
      ///
      HostBridgeInstance->CanRestarted = FALSE;
      return EFI_SUCCESS;
      break;

    case EfiPciHostBridgeEndBusAllocation:
      ///
      /// No specific action is required here, can perform any chipset specific programing
      ///
      return EFI_SUCCESS;
      break;

    case EfiPciHostBridgeBeginResourceAllocation:
      ///
      /// No specific action is required here, can perform any chipset specific programing
      ///
      return EFI_SUCCESS;
      break;

    case EfiPciHostBridgeAllocateResources:
      ReturnStatus = EFI_SUCCESS;
      if (HostBridgeInstance->ResourceSubmited) {
        ///
        /// Take care of the resource dependencies between the root bridges
        ///
        List = HostBridgeInstance->Head.ForwardLink;
        while (List != &HostBridgeInstance->Head) {
          RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
          // AMI_OVERRIDE_SA0021_START >>> EIP191481 Implement the proposal of AMI fxied MMIO.
          Status = HbCspAdjustMemoryMmioOverlap(RootBridgeInstance);
          // AMI_OVERRIDE_SA0021_END <<< EIP191481 Implement the proposal of AMI fxied MMIO.      
          for (Index = TypeIo; Index < TypeBus; Index++) {
            if (RootBridgeInstance->ResAllocNode[Index].Status != ResNone) {
              AddrLen   = RootBridgeInstance->ResAllocNode[Index].Length;
              Alignment = RootBridgeInstance->ResAllocNode[Index].Alignment;

              ///
              /// Get the number of '1' in Alignment.
              ///
              for (BitsOfAlignment = 0; Alignment != 0; BitsOfAlignment++) {
                Alignment = RShiftU64 (Alignment, 1);
              }

              switch (Index) {
                case TypeIo:
                  ///
                  /// It is impossible for this chipset to align 0xFFFF for IO16
                  /// So clear it
                  ///
                  if (BitsOfAlignment >= 16) {
                    BitsOfAlignment = 0;
                  }
                  if ((PcdGet8 (PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchBottomUp) || (PcdGet8 (PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchTopDown)) {
                    BaseAddress = 0xFFFF;
                  }
                  Status = gDS->AllocateIoSpace (
                                  PcdGet8 (PcdEfiGcdAllocateType),
                                  EfiGcdIoTypeIo,
                                  BitsOfAlignment,
                                  AddrLen,
                                  &BaseAddress,
                                  mDriverImageHandle,
                                  NULL
                                  );
                  if (!EFI_ERROR (Status)) {
                    RootBridgeInstance->ResAllocNode[Index].Base    = (UINTN) BaseAddress;
                    RootBridgeInstance->ResAllocNode[Index].Status  = ResAllocated;
                  } else {
                    ///
                    /// Not able to allocate enough I/O memory - critical stop
                    ///
                    DEBUG ((DEBUG_ERROR, "Out of I/O space! AllocateIoSpace() returned %r\n", Status));
                    DEBUG ((DEBUG_ERROR, "Size requested: 0x%lX bytes\n", AddrLen));
                    ReturnStatus = Status;
                  }
                  break;

                case TypeMem32:
                case TypePMem32:
                case TypeMem64:
                case TypePMem64:
                  MmioHasBeenAllocated = FALSE;
                  Status               = EFI_OUT_OF_RESOURCES;
                  BaseAddress          = 0;
                  ///
                  /// It is impossible for this chipset to align 0xFFFFFFFF for Mem32
                  /// So clear it
                  ///
                  if ((Index == TypeMem32) || (Index == TypePMem32)) {
                    if (BitsOfAlignment >= 32) {
                      BitsOfAlignment = 0;
                    }
                  }
#ifndef MINTREE_FLAG
                  ///
                  /// Enable BIOS above 4GB MMIO assignment only when policy enabled.
                  ///
                  if (mEnableAbove4GBMmioBiosAssignemnt == 1) {
                    ///
                    /// It is impossible for this chipset to align 0xFFFFFFFFFFFFFFFF for Mem64
                    /// So clear it
                    ///
                    if ((Index == TypeMem64) || (Index == TypePMem64)) {
                      if (BitsOfAlignment >= 64) {
                        BitsOfAlignment = 0;
                      }
                      DEBUG ((DEBUG_INFO, " Allocating TypeMem64..\n"));
                      BaseAddress = MAX_ADDRESS;
                      Status = gDS->AllocateMemorySpace (
                                      EfiGcdAllocateAnySearchTopDown,
                                      EfiGcdMemoryTypeMemoryMappedIo,
                                      BitsOfAlignment,
                                      AddrLen,
                                      &BaseAddress,
                                      mDriverImageHandle,
                                      NULL
                                      );
                      if (Status == EFI_SUCCESS) {
                        MmioHasBeenAllocated = TRUE;
                      }
                    }
                  }
#endif //MINTREE_FLAG
                  if (MmioHasBeenAllocated == FALSE) {
                    DEBUG ((DEBUG_INFO, " Allocating TypeMem32..\n"));
                    if ((PcdGet8 (PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchBottomUp) || (PcdGet8 (PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchTopDown)) {
                      BaseAddress = 0xFFFFFFFF;
                    }
                    Status = gDS->AllocateMemorySpace (
                                    PcdGet8 (PcdEfiGcdAllocateType),
                                    EfiGcdMemoryTypeMemoryMappedIo,
                                    BitsOfAlignment,
                                    AddrLen,
                                    &BaseAddress,
                                    mDriverImageHandle,
                                    NULL
                                    );
                  }
                  if (!EFI_ERROR (Status)) {
                    ///
                    /// We were able to allocate the PCI memory
                    ///
                    RootBridgeInstance->ResAllocNode[Index].Base    = (UINTN) BaseAddress;
                    RootBridgeInstance->ResAllocNode[Index].Status  = ResAllocated;
                  } else {
                    ///
                    /// Not able to allocate enough PCI memory - critical stop
                    ///
                    DEBUG ((DEBUG_ERROR, "Out of PCI memory! AllocateMemorySpace() returned %r\n", Status));
                    DEBUG ((DEBUG_ERROR, "Size requested: 0x%lX bytes\n", AddrLen));
                    ReturnStatus = Status;
                  }
                  break;
                default:
                  break;
              }
              ///
              /// end switch
              ///
            }
          }

          List = List->ForwardLink;
        }

        return ReturnStatus;
      } else {
        return EFI_NOT_READY;
      }
      break;

    case EfiPciHostBridgeSetResources:
      break;

    case EfiPciHostBridgeFreeResources:
      ReturnStatus  = EFI_SUCCESS;

      List          = HostBridgeInstance->Head.ForwardLink;
      while (List != &HostBridgeInstance->Head) {
        RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
        for (Index = TypeIo; Index < TypeBus; Index++) {
          if (RootBridgeInstance->ResAllocNode[Index].Status == ResAllocated) {
            AddrLen     = RootBridgeInstance->ResAllocNode[Index].Length;
            BaseAddress = RootBridgeInstance->ResAllocNode[Index].Base;
            switch (Index) {
              case TypeIo:
                Status = gDS->FreeIoSpace (BaseAddress, AddrLen);
                break;

              case TypeMem32:
              case TypePMem32:
              case TypeMem64:
              case TypePMem64:
                Status = gDS->FreeMemorySpace (BaseAddress, AddrLen);
                break;

              default:
                Status = EFI_INVALID_PARAMETER;

            }

            if (EFI_ERROR (Status)) {
              ReturnStatus = Status;
            }
            ///
            /// end switch
            ///
            RootBridgeInstance->ResAllocNode[Index].Type    = Index;
            RootBridgeInstance->ResAllocNode[Index].Base    = 0;
            RootBridgeInstance->ResAllocNode[Index].Length  = 0;
            RootBridgeInstance->ResAllocNode[Index].Status  = ResNone;
          }
        }

        List = List->ForwardLink;
      }

      HostBridgeInstance->ResourceSubmited  = FALSE;
      HostBridgeInstance->CanRestarted      = TRUE;
      return ReturnStatus;
      break;

    case EfiPciHostBridgeEndResourceAllocation:
      HostBridgeInstance->CanRestarted = FALSE;
      break;

    default:
      return EFI_INVALID_PARAMETER;
  }
  ///
  /// end switch
  ///
  return EFI_SUCCESS;
}

/**
  Return the device handle of the next PCI root bridge that is associated with
  this Host Bridge

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - Returns the device handle of the next PCI Root Bridge.
                      On input, it holds the RootBridgeHandle returned by the most
                      recent call to GetNextRootBridge().The handle for the first
                      PCI Root Bridge is returned if RootBridgeHandle is NULL on input

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_NOT_FOUND          -  Next PCI root bridge not found.
  @retval EFI_INVALID_PARAMETER  -  Wrong parameter passed in.
**/
EFI_STATUS
EFIAPI
GetNextRootBridge (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN OUT EFI_HANDLE                                   *RootBridgeHandle
  )
{
  BOOLEAN                   NoRootBridge;
  LIST_ENTRY                *List;
  PCI_HOST_BRIDGE_INSTANCE  *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE  *RootBridgeInstance;

  NoRootBridge        = TRUE;
  HostBridgeInstance  = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List                = HostBridgeInstance->Head.ForwardLink;

  while (List != &HostBridgeInstance->Head) {
    NoRootBridge        = FALSE;
    RootBridgeInstance  = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (*RootBridgeHandle == NULL) {
      ///
      /// Return the first Root Bridge Handle of the Host Bridge
      ///
      *RootBridgeHandle = RootBridgeInstance->Handle;
      return EFI_SUCCESS;
    } else {
      if (*RootBridgeHandle == RootBridgeInstance->Handle) {
        ///
        /// Get next if have
        ///
        List = List->ForwardLink;
        if (List != &HostBridgeInstance->Head) {
          RootBridgeInstance  = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
          *RootBridgeHandle   = RootBridgeInstance->Handle;
          return EFI_SUCCESS;
        } else {
          return EFI_NOT_FOUND;
        }
      }
    }

    List = List->ForwardLink;
  }
  ///
  /// end while
  ///
  if (NoRootBridge) {
    return EFI_NOT_FOUND;
  } else {
    return EFI_INVALID_PARAMETER;
  }
}

/**
  Returns the attributes of a PCI Root Bridge.

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance.
  @param[in] RootBridgeHandle  - The device handle of the PCI Root Bridge
                      that the caller is interested in
  @param[in] Attributes        - The pointer to attributes of the PCI Root Bridge

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  Attributes parameter passed in is NULL or
                            RootBridgeHandle is not an EFI_HANDLE
                            that was returned on a previous call to
                            GetNextRootBridge().
**/
EFI_STATUS
EFIAPI
GetAttributes (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT UINT64                                          *Attributes
  )
{
  LIST_ENTRY                *List;
  PCI_HOST_BRIDGE_INSTANCE  *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE  *RootBridgeInstance;

  if (Attributes == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance  = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List                = HostBridgeInstance->Head.ForwardLink;
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
#ifndef MINTREE_FLAG
      if (mEnableAbove4GBMmioBiosAssignemnt == 1) {
        RootBridgeInstance->RootBridgeAttrib |= EFI_PCI_HOST_BRIDGE_MEM64_DECODE;
      }
#endif //MINTREE_FLAG
      *Attributes = RootBridgeInstance->RootBridgeAttrib;
      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }
  ///
  /// RootBridgeHandle is not an EFI_HANDLE
  /// that was returned on a previous call to GetNextRootBridge()
  ///
  return EFI_INVALID_PARAMETER;
}

/**
  This is the request from the PCI enumerator to set up
  the specified PCI Root Bridge for bus enumeration process.

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance.
  @param[in] RootBridgeHandle  - The PCI Root Bridge to be set up.
  @param[in] Configuration     - Pointer to the pointer to the PCI bus resource descriptor.

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_OUT_OF_RESOURCES   -  Not enough pool to be allocated.
  @retval EFI_INVALID_PARAMETER  -  RootBridgeHandle is not a valid handle.
**/
EFI_STATUS
EFIAPI
StartBusEnumeration (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT VOID                                            **Configuration
  )
{
  LIST_ENTRY                *List;
  PCI_HOST_BRIDGE_INSTANCE  *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE  *RootBridgeInstance;
  VOID                      *Buffer;
  UINT8                     *Temp;
  UINT64                    BusStart;
  UINT64                    BusEnd;

  HostBridgeInstance  = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List                = HostBridgeInstance->Head.ForwardLink;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      ///
      /// Set up the Root Bridge for Bus Enumeration
      ///
      BusStart  = RootBridgeInstance->BusBase;
      BusEnd    = RootBridgeInstance->BusLimit;

      ///
      /// Program the Hardware(if needed) if error return EFI_DEVICE_ERROR
      ///
      Buffer = AllocatePool (sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR));
      if (Buffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      Temp  = (UINT8 *) Buffer;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->Desc                  = ACPI_ADDRESS_SPACE_DESCRIPTOR;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->Len                   = 0x2B;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->ResType               = ACPI_ADDRESS_SPACE_TYPE_BUS;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->GenFlag               = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->SpecificFlag          = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->AddrSpaceGranularity  = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->AddrRangeMin          = BusStart;
// AMI_OVERRIDE_SA0021_START >>>
#if 0
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->AddrRangeMax          = 0;
#else
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->AddrRangeMax          = BusEnd;      
#endif
// AMI_OVERRIDE_SA0021_END <<<
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->AddrTranslationOffset = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->AddrLen               = BusEnd - BusStart + 1;

      Temp = Temp + sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);

      ((EFI_ACPI_END_TAG_DESCRIPTOR *) Temp)->Desc = ACPI_END_TAG_DESCRIPTOR;
      ((EFI_ACPI_END_TAG_DESCRIPTOR *) Temp)->Checksum = 0x0;
      *Configuration = Buffer;
      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
  This function programs the PCI Root Bridge hardware so that
  it decodes the specified PCI bus range

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - The PCI Root Bridge whose bus range is to be programmed
  @param[in] Configuration     - The pointer to the PCI bus resource descriptor

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  Wrong parameters passed in.
**/
EFI_STATUS
EFIAPI
SetBusNumbers (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  IN VOID                                             *Configuration
  )
{
  LIST_ENTRY                *List;
  PCI_HOST_BRIDGE_INSTANCE  *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE  *RootBridgeInstance;
  UINT8                     *Ptr;
  UINTN                     BusStart;
  UINTN                     BusEnd;
  UINTN                     BusLen;

  if (Configuration == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Ptr = Configuration;

  ///
  /// Check the Configuration is valid
  ///
  if (*Ptr != ACPI_ADDRESS_SPACE_DESCRIPTOR) {
    return EFI_INVALID_PARAMETER;
  }

  if (((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Ptr)->ResType != ACPI_ADDRESS_SPACE_TYPE_BUS) {
    return EFI_INVALID_PARAMETER;
  }

  Ptr += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
  if (*Ptr != ACPI_END_TAG_DESCRIPTOR) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance  = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List                = HostBridgeInstance->Head.ForwardLink;
  Ptr                 = Configuration;
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      BusStart  = (UINTN) ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Ptr)->AddrRangeMin;
      BusLen    = (UINTN) ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Ptr)->AddrLen;
      BusEnd    = BusStart + BusLen - 1;
      if (BusStart > BusEnd) {
        return EFI_INVALID_PARAMETER;
      }

      if ((BusStart < RootBridgeInstance->BusBase) || (BusEnd > RootBridgeInstance->BusLimit)) {
        return EFI_INVALID_PARAMETER;
      }
      ///
      /// Update the Bus Range
      ///
      RootBridgeInstance->ResAllocNode[TypeBus].Base    = BusStart;
      RootBridgeInstance->ResAllocNode[TypeBus].Length  = BusLen;
      RootBridgeInstance->ResAllocNode[TypeBus].Status  = ResAllocated;

      ///
      /// Program the Root Bridge Hardware
      ///
      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
  Submits the I/O and memory resource requirements for the specified PCI Root Bridge

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - The PCI Root Bridge whose I/O and memory resource requirements
                      are being submitted
  @param[in] Configuration     - The pointer to the PCI I/O and PCI memory resource descriptor

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  Wrong parameters passed in.
**/
EFI_STATUS
EFIAPI
SubmitResources (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  IN VOID                                             *Configuration
  )
{
  LIST_ENTRY                        *List;
  PCI_HOST_BRIDGE_INSTANCE          *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE          *RootBridgeInstance;
  UINT8                             *Temp;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *ptr;
  UINT64                            AddrLen;
  UINT64                            Alignment;

  ///
  /// Check the input parameter: Configuration
  ///
  if (Configuration == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance  = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List                = HostBridgeInstance->Head.ForwardLink;
  Temp                = (UINT8 *) Configuration;
  while (*Temp == ACPI_ADDRESS_SPACE_DESCRIPTOR) {
    Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
  }

  if (*Temp != ACPI_END_TAG_DESCRIPTOR) {
    return EFI_INVALID_PARAMETER;
  }

  Temp = (UINT8 *) Configuration;
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      while (*Temp == ACPI_ADDRESS_SPACE_DESCRIPTOR) {
        ptr = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp;

// AMI_OVERRIDE_SA0013_START >>> Fixed system hang when insert PCIE 8GB Resources Crad.
        if (mEnableAbove4GBMmioBiosAssignemnt == 0) {
        ///
        /// Check address range alignment
        ///
#if 0
        if (ptr->AddrRangeMax != (Power2MaxMemory (ptr->AddrRangeMax + 1) - 1)) {
#else
          if (ptr->AddrRangeMax > (UINT64) 0x100000000 ||
            ptr->AddrRangeMax != (Power2MaxMemory (ptr->AddrRangeMax + 1) - 1)
            ) {
#endif
//-----------------------------------------------------------------------
// AMI_OVERRIDE_SA0013_START >>> OUT of Resources Generic support start..
//-----------------------------------------------------------------------
#if BoardPciRes_SUPPORT == 1
            AMI_OUT_OF_RES_VAR  OutOfResVar;
            EFI_STATUS                Status;
            //--------------------------------------
            //Check if OUT_OF_RES_VAR was already created.
            Status=AmiPciOutOfRes(&OutOfResVar, TRUE);
            if(EFI_ERROR(Status)) SetMem(&OutOfResVar,sizeof(AMI_OUT_OF_RES_VAR),0);
            else OutOfResVar.Count++;
            OutOfResVar.Resource=*(ASLR_QWORD_ASD*)ptr;
            Status=AmiPciOutOfRes(&OutOfResVar, FALSE); 
#endif
//-----------------------------------------------------------------------
// AMI_OVERRIDE_SA0013_END <<< OUT of Resources Generic support end.
//-----------------------------------------------------------------------
          return EFI_INVALID_PARAMETER;
          }
        }
// AMI_OVERRIDE_SA0013_END <<< Fixed system hang when insert PCIE 8GB Resources Crad.

        switch (ptr->ResType) {
          case ACPI_ADDRESS_SPACE_TYPE_MEM:
            ///
            /// check the memory resource request is supported by PCI root bridge
            ///
            if (((RootBridgeInstance->RootBridgeAttrib & EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM) == EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM) && (ptr->SpecificFlag == 0x06)) {
              return EFI_INVALID_PARAMETER;
            }

            AddrLen   = (UINT64) ptr->AddrLen;
            Alignment = (UINT64) ptr->AddrRangeMax;
            if (ptr->AddrSpaceGranularity == 32) {
              if ((ptr->SpecificFlag & 0x06) == 0x06) {
                ///
                /// Apply from GCD
                ///
                RootBridgeInstance->ResAllocNode[TypePMem32].Length     = AddrLen;
                RootBridgeInstance->ResAllocNode[TypePMem32].Alignment  = Alignment;
                RootBridgeInstance->ResAllocNode[TypePMem32].Status     = ResRequested;
                HostBridgeInstance->ResourceSubmited                    = TRUE;

              } else {
                RootBridgeInstance->ResAllocNode[TypeMem32].Length    = AddrLen;
                RootBridgeInstance->ResAllocNode[TypeMem32].Alignment = Alignment;
                RootBridgeInstance->ResAllocNode[TypeMem32].Status    = ResRequested;
                HostBridgeInstance->ResourceSubmited                  = TRUE;
              }
            }

            if (ptr->AddrSpaceGranularity == 64) {
              if ((RootBridgeInstance->RootBridgeAttrib & EFI_PCI_HOST_BRIDGE_MEM64_DECODE) == 0) {
                return EFI_INVALID_PARAMETER;
              } else {
                if ((ptr->SpecificFlag & 0x06) == 0x06) {
                  RootBridgeInstance->ResAllocNode[TypePMem64].Length     = AddrLen;
                  RootBridgeInstance->ResAllocNode[TypePMem64].Alignment  = Alignment;
                  RootBridgeInstance->ResAllocNode[TypePMem64].Status     = ResSubmitted;
                  HostBridgeInstance->ResourceSubmited                    = TRUE;

                } else {
                  RootBridgeInstance->ResAllocNode[TypeMem64].Length    = AddrLen;
                  RootBridgeInstance->ResAllocNode[TypeMem64].Alignment = Alignment;
                  RootBridgeInstance->ResAllocNode[TypeMem64].Status    = ResSubmitted;
                  HostBridgeInstance->ResourceSubmited                  = TRUE;
                }
              }
            }
            break;

          case ACPI_ADDRESS_SPACE_TYPE_IO:
            AddrLen   = (UINT64) ptr->AddrLen;
            Alignment = (UINT64) ptr->AddrRangeMax;
            RootBridgeInstance->ResAllocNode[TypeIo].Length     = AddrLen;
            RootBridgeInstance->ResAllocNode[TypeIo].Alignment  = Alignment;
            RootBridgeInstance->ResAllocNode[TypeIo].Status     = ResRequested;
            HostBridgeInstance->ResourceSubmited                = TRUE;
            break;

          default:
            break;
        }

        Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
      }

      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
  This function returns the proposed resource settings for the specified
  PCI Root Bridge

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - The PCI Root Bridge handle
  @param[in] Configuration     - The pointer to the pointer to the PCI I/O
                      and memory resource descriptor

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_OUT_OF_RESOURCES   -  Not enough pool to be allocated.
  @retval EFI_INVALID_PARAMETER  -  RootBridgeHandle is not a valid handle.
**/
EFI_STATUS
EFIAPI
GetProposedResources (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT VOID                                            **Configuration
  )
{
  LIST_ENTRY                        *List;
  PCI_HOST_BRIDGE_INSTANCE          *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE          *RootBridgeInstance;
  UINTN                             Index;
  UINTN                             Number;
  VOID                              *Buffer;
  UINT8                             *Temp;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *ptr;
  UINT64                            ResStatus;

  Buffer  = NULL;
  Number  = 0;

  ///
  /// Get the Host Bridge Instance from the resource allocation protocol
  ///
  HostBridgeInstance  = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List                = HostBridgeInstance->Head.ForwardLink;

  ///
  /// Enumerate the root bridges in this host bridge
  ///
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      for (Index = 0; Index < TypeBus; Index++) {
        if (RootBridgeInstance->ResAllocNode[Index].Status != ResNone) {
          Number++;
        }
      }

      if (Number > 0) {
        Buffer = AllocatePool (Number * sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR));
        if (Buffer == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }

        ZeroMem (Buffer, sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) * Number + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR));

        Temp = Buffer;
        for (Index = 0; Index < TypeBus; Index++) {
          if (RootBridgeInstance->ResAllocNode[Index].Status != ResNone) {
            ptr       = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp;
            ResStatus = RootBridgeInstance->ResAllocNode[Index].Status;
            switch (Index) {
              case TypeIo:
                ///
                /// Io
                ///
                ptr->Desc                   = 0x8A;
                ptr->Len                    = 0x2B;
                ptr->ResType                = 1;
                ptr->GenFlag                = 0;
                ptr->SpecificFlag           = 0;
                ptr->AddrRangeMin           = RootBridgeInstance->ResAllocNode[Index].Base;
                ptr->AddrRangeMax           = 0;
                ptr->AddrTranslationOffset  = (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
                ptr->AddrLen                = RootBridgeInstance->ResAllocNode[Index].Length;
                break;

              case TypeMem32:
                ///
                /// Memory 32
                ///
                ptr->Desc                   = 0x8A;
                ptr->Len                    = 0x2B;
                ptr->ResType                = 0;
                ptr->GenFlag                = 0;
                ptr->SpecificFlag           = 0;
                ptr->AddrSpaceGranularity   = 32;
                ptr->AddrRangeMin           = RootBridgeInstance->ResAllocNode[Index].Base;
                ptr->AddrRangeMax           = 0;
                ptr->AddrTranslationOffset  = (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
                ptr->AddrLen                = RootBridgeInstance->ResAllocNode[Index].Length;
                break;

              case TypePMem32:
                ///
                /// Prefetch memory 32
                ///
                ptr->Desc                   = 0x8A;
                ptr->Len                    = 0x2B;
                ptr->ResType                = 0;
                ptr->GenFlag                = 0;
                ptr->SpecificFlag           = 6;
                ptr->AddrSpaceGranularity   = 32;
                ptr->AddrRangeMin           = 0;
                ptr->AddrRangeMax           = 0;
                ptr->AddrTranslationOffset  = (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
                ptr->AddrLen                = RootBridgeInstance->ResAllocNode[Index].Length;
                break;

              case TypeMem64:
                ///
                /// Memory 64
                ///
                ptr->Desc                   = 0x8A;
                ptr->Len                    = 0x2B;
                ptr->ResType                = 0;
                ptr->GenFlag                = 0;
                ptr->SpecificFlag           = 0;
                ptr->AddrSpaceGranularity   = 64;
                ptr->AddrRangeMin           = RootBridgeInstance->ResAllocNode[Index].Base;
                ptr->AddrRangeMax           = 0;
                ptr->AddrTranslationOffset  = (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
                ptr->AddrLen                = RootBridgeInstance->ResAllocNode[Index].Length;
                break;

              case TypePMem64:
                ///
                /// Prefetch memory 64
                ///
                ptr->Desc                   = 0x8A;
                ptr->Len                    = 0x2B;
                ptr->ResType                = 0;
                ptr->GenFlag                = 0;
                ptr->SpecificFlag           = 6;
                ptr->AddrSpaceGranularity   = 64;
                ptr->AddrRangeMin           = 0;
                ptr->AddrRangeMax           = 0;
                ptr->AddrTranslationOffset  = (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
                ptr->AddrLen                = RootBridgeInstance->ResAllocNode[Index].Length;
                break;
            }

            Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
          }
        }

        ((EFI_ACPI_END_TAG_DESCRIPTOR *) Temp)->Desc      = 0x79;
        ((EFI_ACPI_END_TAG_DESCRIPTOR *) Temp)->Checksum  = 0x0;
        *Configuration = Buffer;
      }

      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
  This function is called for all the PCI controllers that the PCI
  bus driver finds. Can be used to Preprogram the controller.

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - The PCI Root Bridge handle
  @param[in] PciAddress        - Address of the controller on the PCI bus
  @param[in] Phase             - The Phase during resource allocation

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  RootBridgeHandle is not a valid handle.
**/
EFI_STATUS
EFIAPI
PreprocessController (
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL          *This,
  IN  EFI_HANDLE                                                RootBridgeHandle,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS               PciAddress,
  IN  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE              Phase
  )
{
  PCI_HOST_BRIDGE_INSTANCE  *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE  *RootBridgeInstance;
  LIST_ENTRY                *List;

  HostBridgeInstance  = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List                = HostBridgeInstance->Head.ForwardLink;

  ///
  /// Enumerate the root bridges in this host bridge
  ///
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
  Calculate max memory of power 2

  @param[in] MemoryLength  -  Input memory length.

  @retval Returned Maximum length.
**/
UINT64
Power2MaxMemory (
  IN UINT64                     MemoryLength
  )
{
  UINT64  Result;

  if (RShiftU64 (MemoryLength, 32)) {
    Result = LShiftU64 ((UINT64) GetPowerOfTwo64 (RShiftU64 (MemoryLength, 32)), 32);
  } else {
    Result = (UINT64) GetPowerOfTwo64 (MemoryLength);
  }

  return Result;
}
