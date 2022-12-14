/** @file
  Implementation of loading microcode on processors.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CpuMpPei.h"
// AMI_OVERRIDE_CPU0009_START >>> Get Microcode patch address from Cpu policy
#include <Ppi/SiPolicy.h>

SI_POLICY_PPI	*gSiCpuPolicy                  = NULL;
// AMI_OVERRIDE_CPU0009_END <<<
/**
  Get microcode update signature of currently loaded microcode update.

  @return  Microcode signature.

**/
UINT32
GetCurrentMicrocodeSignature (
  VOID
  )
{
  UINT64 Signature;

  AsmWriteMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID, 0);
  AsmCpuid (CPUID_VERSION_INFO, NULL, NULL, NULL, NULL);
  Signature = AsmReadMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID);
  return (UINT32) RShiftU64 (Signature, 32);
}

/**
  Detect whether specified processor can find matching microcode patch and load it.

  @param PeiCpuMpData        Pointer to PEI CPU MP Data
**/
VOID
MicrocodeDetect (
  IN PEI_CPU_MP_DATA            *PeiCpuMpData
  )
{
  UINT64                                  MicrocodePatchAddress;
// AMI_OVERRIDE_CPU0009_START >>> Get Microcode patch address from Cpu policy
#if 0
  UINT64                                  MicrocodePatchRegionSize;
#endif
// AMI_OVERRIDE_CPU0009_END <<<
  UINT32                                  ExtendedTableLength;
  UINT32                                  ExtendedTableCount;
  EFI_CPU_MICROCODE_EXTENDED_TABLE        *ExtendedTable;
  EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER *ExtendedTableHeader;
  EFI_CPU_MICROCODE_HEADER                *MicrocodeEntryPoint;
// AMI_OVERRIDE_CPU0009_START >>> Get Microcode patch address from Cpu policy
#if 0
  UINTN                                   MicrocodeEnd;
#endif
// AMI_OVERRIDE_CPU0009_END <<<
  UINTN                                   Index;
  UINT8                                   PlatformId;
  UINT32                                  RegEax;
  UINT32                                  CurrentRevision;
  UINT32                                  LatestRevision;
  UINTN                                   TotalSize;
  UINT32                                  CheckSum32;
  BOOLEAN                                 CorrectMicrocode;
  MICROCODE_INFO                          MicrocodeInfo;
// AMI_OVERRIDE_CPU0009_START >>> Get Microcode patch address from Cpu policy
  EFI_STATUS           					  Status;
  CPU_CONFIG                  			  *CpuConfig;
// AMI_OVERRIDE_CPU0009_END <<<

// AMI_OVERRIDE_CPU0009_START >>> Get Microcode patch address from Cpu policy
  Status = PeiServicesLocatePpi (
		   &gSiPolicyPpiGuid,
		   0,
		   NULL,
		   (VOID **) &gSiCpuPolicy);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) return;

  Status = GetConfigBlock ((VOID *) gSiCpuPolicy, &gCpuConfigGuid, (VOID *) &CpuConfig);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) return;
  
  if (CpuConfig->MicrocodePatchAddress == 0) {
	//
	// There is no microcode patches
	//
	return;
  }
  
  MicrocodePatchAddress = (UINT64)CpuConfig->MicrocodePatchAddress;
// AMI_OVERRIDE_CPU0009_END <<<

  ZeroMem (&MicrocodeInfo, sizeof (MICROCODE_INFO));

// AMI_OVERRIDE_CPU0009_START >>> Get Microcode patch address from Cpu policy
#if 0
  MicrocodePatchAddress    = PcdGet64 (PcdCpuMicrocodePatchAddress);
  MicrocodePatchRegionSize = PcdGet64 (PcdCpuMicrocodePatchRegionSize);
  if (MicrocodePatchRegionSize == 0) {
    //
    // There is no microcode patches
    //
    return;
  }
#endif
// AMI_OVERRIDE_CPU0009_END <<<

  CurrentRevision = GetCurrentMicrocodeSignature ();
  if (CurrentRevision != 0) {
    //
    // Skip loading microcode if it has been loaded successfully
    //
    return;
  }

  ExtendedTableLength = 0;
  //
  // Here data of CPUID leafs have not been collected into context buffer, so
  // GetProcessorCpuid() cannot be used here to retrieve CPUID data.
  //
  AsmCpuid (CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);

  //
  // The index of platform information resides in bits 50:52 of MSR IA32_PLATFORM_ID
  //
  PlatformId = (UINT8) AsmMsrBitFieldRead64 (EFI_MSR_IA32_PLATFORM_ID, 50, 52);

  LatestRevision = 0;
// AMI_OVERRIDE_CPU0009_START >>> Get Microcode patch address from Cpu policy
#if 0
  MicrocodeEnd = (UINTN) (MicrocodePatchAddress + MicrocodePatchRegionSize);
#endif
// AMI_OVERRIDE_CPU0009_END <<<
  MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *) (UINTN) MicrocodePatchAddress;
// AMI_OVERRIDE_CPU0009_START >>> Get Microcode patch address from Cpu policy
#if 0
  do {
#endif
// AMI_OVERRIDE_CPU0009_END <<<
    //
    // Check if the microcode is for the Cpu and the version is newer
    // and the update can be processed on the platform
    //
    CorrectMicrocode = FALSE;
    if (MicrocodeEntryPoint->HeaderVersion == 0x1) {
      //
      // It is the microcode header. It is not the padding data between microcode patches
      // becasue the padding data should not include 0x00000001 and it should be the repeated
      // byte format (like 0xXYXYXYXY....).
      //
      if (MicrocodeEntryPoint->ProcessorId == RegEax &&
          MicrocodeEntryPoint->UpdateRevision > LatestRevision &&
          (MicrocodeEntryPoint->ProcessorFlags & (1 << PlatformId))
          ) {
        if (MicrocodeEntryPoint->DataSize == 0) {
          CheckSum32 = CalculateSum32 ((UINT32 *)MicrocodeEntryPoint, 2048);
        } else {
          CheckSum32 = CalculateSum32 ((UINT32 *)MicrocodeEntryPoint, MicrocodeEntryPoint->DataSize + sizeof(EFI_CPU_MICROCODE_HEADER));
        }
        if (CheckSum32 == 0) {
          CorrectMicrocode = TRUE;
        }
      } else if ((MicrocodeEntryPoint->DataSize != 0) &&
                 (MicrocodeEntryPoint->UpdateRevision > LatestRevision)) {
        ExtendedTableLength = MicrocodeEntryPoint->TotalSize - (MicrocodeEntryPoint->DataSize + sizeof (EFI_CPU_MICROCODE_HEADER));
        if (ExtendedTableLength != 0) {
          //
          // Extended Table exist, check if the CPU in support list
          //
          ExtendedTableHeader = (EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER *)((UINT8 *)(MicrocodeEntryPoint) + MicrocodeEntryPoint->DataSize + sizeof (EFI_CPU_MICROCODE_HEADER));
          //
          // Calculate Extended Checksum
          //
          if ((ExtendedTableLength % 4) == 0) {
            CheckSum32 = CalculateSum32 ((UINT32 *)ExtendedTableHeader, ExtendedTableLength);
            if (CheckSum32 == 0) {
              //
              // Checksum correct
              //
              ExtendedTableCount = ExtendedTableHeader->ExtendedSignatureCount;
              ExtendedTable      = (EFI_CPU_MICROCODE_EXTENDED_TABLE *)(ExtendedTableHeader + 1);
              for (Index = 0; Index < ExtendedTableCount; Index ++) {
                CheckSum32 = CalculateSum32 ((UINT32 *)ExtendedTable, sizeof(EFI_CPU_MICROCODE_EXTENDED_TABLE));
                if (CheckSum32 == 0) {
                  //
                  // Verify Header
                  //
                  if ((ExtendedTable->ProcessorSignature == RegEax) &&
                      (ExtendedTable->ProcessorFlag & (1 << PlatformId)) ) {
                    //
                    // Find one
                    //
                    CorrectMicrocode = TRUE;
                    break;
                  }
                }
                ExtendedTable ++;
              }
            }
          }
        }
      }
    } else {
      //
      // It is the padding data between the microcode patches for microcode patches alignment.
      // Because the microcode patch is the multiple of 1-KByte, the padding data should not
      // exist if the microcode patch alignment value is not larger than 1-KByte. So, the microcode
      // alignment value should be larger than 1-KByte. We could skip SIZE_1KB padding data to
      // find the next possible microcode patch header.
      //
      MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *) (((UINTN) MicrocodeEntryPoint) + SIZE_1KB);
// AMI_OVERRIDE_CPU0009_START >>> Get Microcode patch address from Cpu policy
#if 0
      continue;
#endif
// AMI_OVERRIDE_CPU0009_END <<<
    }
    //
    // Get the next patch.
    //
    if (MicrocodeEntryPoint->DataSize == 0) {
      TotalSize = 2048;
    } else {
      TotalSize = MicrocodeEntryPoint->TotalSize;
    }

    if (CorrectMicrocode) {
      LatestRevision = MicrocodeEntryPoint->UpdateRevision;
      MicrocodeInfo.MicrocodeData = (VOID *)((UINTN)MicrocodeEntryPoint + sizeof (EFI_CPU_MICROCODE_HEADER));
      MicrocodeInfo.MicrocodeSize = TotalSize;
      MicrocodeInfo.ProcessorId = RegEax;
    }

// AMI_OVERRIDE_CPU0009_START >>> Get Microcode patch address from Cpu policy
#if 0
    MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *) (((UINTN) MicrocodeEntryPoint) + TotalSize);
  } while (((UINTN) MicrocodeEntryPoint < MicrocodeEnd));
#endif
// AMI_OVERRIDE_CPU0009_END <<<

  if (LatestRevision > CurrentRevision) {
    //
    // BIOS only authenticate updates that contain a numerically larger revision
    // than the currently loaded revision, where Current Signature < New Update
    // Revision. A processor with no loaded update is considered to have a
    // revision equal to zero.
    //
    AsmWriteMsr64 (
      EFI_MSR_IA32_BIOS_UPDT_TRIG,
      (UINT64) (UINTN) MicrocodeInfo.MicrocodeData
      );
    //
    // Get and check new microcode signature
    //
    CurrentRevision = GetCurrentMicrocodeSignature ();
    if (CurrentRevision != LatestRevision) {
      AcquireSpinLock(&PeiCpuMpData->MpLock);
      DEBUG ((EFI_D_ERROR, "Updated microcode signature [0x%08x] does not match \
                loaded microcode signature [0x%08x]\n", CurrentRevision, LatestRevision));
      ReleaseSpinLock(&PeiCpuMpData->MpLock);
    }
  }
}
