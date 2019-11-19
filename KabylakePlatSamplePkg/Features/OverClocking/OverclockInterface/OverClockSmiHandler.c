/** @file
  Implements SW SMI communication interface for OS Application Iron city

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2018 Intel Corporation.

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

@par Specification Reference:
**/

//
// External include files do NOT need to be explicitly specified in real EDKII
// environment
//
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
//AMI_OVERRIDE_START - Override for AMI SMM function to access CPU register
//#include <Library/SmmMemLib.h>
//AMI_OVERRIDE_END - Override for AMI SMM function to access CPU register
#include <Protocol/SmmVariable.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmCpuSaveState.h>
#include "OverClockSmiHandler.h"
#include <CpuDataStruct.h>
#include <SetupVariable.h>
#include <OcSetupVariable.h>
#include <Protocol/GlobalNvsArea.h>
//AMI_OVERRIDE_START - Fix waring messages in BET tool.
#include <PlatformInfo.h>
//AMI_OVERRIDE_END - Fix waring messages in BET tool.
//AMI_OVERRIDE_START - Overclocking part needs to be updated for SA50011 [ EIP437916 ]
#include <Library/AmiBufferValidationLib.h>
//AMI_OVERRIDE_END - Overclocking part needs to be updated for SA50011 [ EIP437916 ]

#define MSR_CORE_THREAD_COUNT               0x35
#define MSR_IA32_MISC_ENABLE                0x1a0
#define TURBO_DISABLE_MASK                  ((UINT64) 1 << 38)
#define TURBO_MODE_DISABLE_BIT              38
#define MSR_PLATFORM_INFO                   0xce
#define XE_TDP_TDC_PROGRAMMABLE_BIT         29
#define XE_CORE_RATIO_PROGRAMMABLE_BIT      28
#define MAX_NON_TURBO_RATIO_OFFSET          8
#define MAX_EFFICIENCY_RATIO_OFFSET         40
#define MAX_NON_TURBO_RATIO_MASK            0xff
#define MAX_EFFICIENCY_RATIO_MASK           0xff
#define PROG_TDP_LIMITS                     (1 << 29)
#define PROG_RATIO_LIMITS                   (1 << 28)
#define PROG_TCC_ACTIVATION_OFFSET          (1 << 30)
#define XTU_BUFFER_LENGTH                   (3 * 1024)

#define SYNC_SMI_BIT  (1 << 0)

//AMI_OVERRIDE_START - Override for AMI SMM access
#include <AmiDxeLib.h>
#include <Protocol/SmmCpu.h>
EFI_SMM_CPU_PROTOCOL            *gSmmCpu=NULL;
//AMI_OVERRIDE_END - Override for AMI SMM access
//
// Global variables
//
GLOBAL_REMOVE_IF_UNREFERENCED UINT16                    mNumberOfActiveCores = 0;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_VARIABLE_PROTOCOL *mSmmVariable;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                     mXtuSmiBaseAddress;
EFI_GLOBAL_NVS_AREA_PROTOCOL  *mGlobalNvsAreaProtocol;
EFI_GLOBAL_NVS_AREA           *mGlobalNvsAreaPtr;
//AMI_OVERRIDE_START - Fix waring messages in BET tool.
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                         PlatformFlavor = 0;
GLOBAL_REMOVE_IF_UNREFERENCED UINT32                        NO_OF_BIOS_SETTING_STATIC_ENTRIES;
//AMI_OVERRIDE_END - Fix waring messages in BET tool.
//AMI_OVERRIDE_START - Override for AMI SMM access
extern EFI_GUID  gSetupVariableGuid;
//AMI_OVERRIDE_END - Override for AMI SMM access

/**
  Determine if CPU supports Turbo mode.

**/
BOOLEAN isTurboModeSupported (
  VOID
  )
{
  BOOLEAN ret;
  EFI_CPUID_REGISTER            Cpuid = { 0, 0, 0, 0 };
  MSR_REGISTER MsrData;

  MsrData.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);

  AsmCpuid (6, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  ret = (BOOLEAN)((((Cpuid.RegEax) & 2) != 0) ||
                 (((MsrData.Qword & TURBO_DISABLE_MASK)) != 0)
                );
  return ret;
}

/**
  Determine if CPU supports Programmable TDC/TDP Limit for the Turbo mode.

**/
BOOLEAN isXETdcTdpLimitSupported (
  VOID
  )
{
  BOOLEAN ret;
  UINT64 MsrData = AsmReadMsr64(MSR_PLATFORM_INFO);
  ret = (UINT32) (MsrData & (PROG_TDP_LIMITS)) ? 1:0;
  return ret;
}

/**
  Determine if CPU supports Programmable Core Ratio Limit for the Turbo mode.

**/
BOOLEAN isCoreRatioLimitSupported (
  VOID
  )
{
  BOOLEAN ret;
  UINT64 MsrData = AsmReadMsr64 (MSR_PLATFORM_INFO);
  ret = (UINT32) (MsrData & (PROG_RATIO_LIMITS)) ? 1:0;
  return ret;
}

/**
  This function is the SMM handler. It reads or write BIOS settings
  data with respect to BIOS settings command.

  @param[in] DispatchHandle      The handle of the dispatch
  @param[in] DispatchContext     A Pointer to EFI_SMM_SW_REGISTER_CONTEXT
  @param[in] CommBuffer          A Pointer to
  @param[in] CommBufferSize      A Pointer to

**/
EFI_STATUS
EFIAPI
SMIFlashSMIHandler (
  IN         EFI_HANDLE                 DispatchHandle,
  IN  CONST  VOID                       *DispatchContext,
  IN  OUT    VOID                       *CommBuffer,
  IN  OUT    UINTN                      *CommBufferSize
  )
{
  EFI_STATUS                  Status;
  UINT32                      ReadWrite;
  UINT32                      TimerValue;
  UINTN                       OcVariableSize;
  UINTN                       Cpu;
  BIOS_SETTINGS_DATA_HEADER   *MemoryDataLocation;
  OC_SETUP_DATA               OcSetupData;
  UINT32                      Length;
  BOOLEAN                     TurboModeSupported;
  BOOLEAN                     TurboTdpLimitProgrammable;
  BOOLEAN                     CoreRatioLimitAvailable;
  UINTN                       HeaderLength;
  UINT8                       Index;
  UINT32                      Count;
  UINT32                      BiosSettingValue;
  BIOS_SETTING_ENTRY          *BiosSettingEntries;
  UINT8                       OverclockingLock;
//AMI_OVERRIDE_START - Override for AMI SMM access
#if 0
  UINT32                      Smbase;
  UINT32                      SmmIoMiscInfo;
  EFI_SMM_CPU_STATE           *pCpuSaveState;
  EFI_SMM_CPU_STATE           *CpuSaveState;
  UINTN                       CpuIndex;
#else
  UINT32                      ErrorCode = 0;
  UINT32                  	  Attributes = 0;
  UINT32                      SaAttributes = 0;  
  UINT32                      CpuAttributes = 0;
#endif
//AMI_OVERRIDE_END - Override for AMI SMM access
  UINT32                      OcVariableAttributes;
  UINT32                      MaxEntries;
//AMI_OVERRIDE_START - Fix EIP229470  
  UINT32                      DdrRatioValue = 0;
//AMI_OVERRIDE_END - Fix EIP229470

  OcVariableSize = sizeof (OC_SETUP_DATA);
  Length = 0;
  Index = 0;
  OverclockingLock = 0;
  Count = 0;
  HeaderLength = 0;

  Status = mSmmVariable->SmmGetVariable (
                            L"OcSetup",
                            &gOcSetupVariableGuid,
                            &OcVariableAttributes,
                            &OcVariableSize,
                            &OcSetupData
                           );
  ASSERT_EFI_ERROR (Status);

  // Do not process XTU SMI requests if overclocking support is disabled
  if (EFI_ERROR (Status) || OcSetupData.OverclockingSupport == 0) {
    DEBUG ((DEBUG_INFO, "OC SMI: Overclocking is disabled.\n"));
    return Status;
  }

  DEBUG ((DEBUG_INFO, "RegisterProtocolOverClockingInterface entry\n"));

//AMI_OVERRIDE_START - Override for AMI SMM access
#if 0
  CpuSaveState = NULL;
  for (CpuIndex = 0; CpuIndex < gSmst->NumberOfCpus; CpuIndex++) {
    //
    // Find which CPU caused the SW SMI
    //
    Smbase = ((EFI_SMM_CPU_STATE *)(gSmst->CpuSaveState[CpuIndex]))->x64.SMBASE;

    SmmIoMiscInfo = *(UINT32 *) (((UINT8 *) (UINTN) Smbase) + 0x8000 + 0x7FA4);

    if (((SmmIoMiscInfo >> 16) == 0xB2) && (SmmIoMiscInfo & SYNC_SMI_BIT)) {
      CpuSaveState = (EFI_SMM_CPU_STATE *)(&(gSmst->CpuSaveState[CpuIndex]));
      break;
    }
  }
  ASSERT (CpuSaveState != NULL);

  Cpu = CpuIndex;

  //
  // Get the BIOS Settings command (ECX) and 32-bit Physical Memory Data Location (EBX)
  // of the Setting Data structure from the CPU save state.
  //
  pCpuSaveState              = (EFI_SMM_CPU_STATE *)(gSmst->CpuSaveState[Cpu]);
  ReadWrite                  = (UINT32) (pCpuSaveState->x64)._RCX;
  TimerValue                 = (UINT32) (pCpuSaveState->x64)._RBX;
  MemoryDataLocation         = (BIOS_SETTINGS_DATA_HEADER*) (UINTN) (pCpuSaveState->x64)._RBX;
#endif
//AMI_OVERRIDE_START - Overclocking part needs to be updated for SA50011 [ EIP437916 ]  
    if (CommBuffer == NULL || CommBufferSize == NULL) return EFI_INVALID_PARAMETER;
    if (*CommBufferSize == 0) return EFI_INVALID_PARAMETER;

    Cpu = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->SwSmiCpuIndex;
//AMI_OVERRIDE_END - Overclocking part needs to be updated for SA50011 [ EIP437916 ]
    //
    // Found Invalid CPU number, return
    //
    if(Cpu == (UINTN)-1) return EFI_UNSUPPORTED;

    Status = gSmmCpu->ReadSaveState ( gSmmCpu, \
                                      4, \
                                      EFI_SMM_SAVE_STATE_REGISTER_RBX, \
                                      Cpu, \
                                      &TimerValue );
    Status = gSmmCpu->ReadSaveState ( gSmmCpu, \
                                      4, \
                                      EFI_SMM_SAVE_STATE_REGISTER_RCX, \
                                      Cpu, \
                                      &ReadWrite );
    
    DEBUG((EFI_D_INFO, "SMI Handler : Command = 0x%x %s%s\n", ReadWrite, (ReadWrite == 0 ? "(Read Bios Setting command)" : ""), (ReadWrite == 1 ? "(Write Bios Setting command)" : "")));
    DEBUG((EFI_D_INFO, "SMI Handler : ECX value = 0x%08X\n", TimerValue));
      
    MemoryDataLocation         = (BIOS_SETTINGS_DATA_HEADER*) (UINTN) TimerValue;    
//AMI_OVERRIDE_END - Override for AMI SMM access
  BiosSettingEntries = (BIOS_SETTING_ENTRY*)((UINTN)MemoryDataLocation + sizeof(BIOS_SETTINGS_DATA_HEADER));

//AMI_OVERRIDE_START - Override for AMI SMM function to access CPU register
#if 0
  //
  //  Verify MemoryDataLocation is the same address that was allocated during POST
  //
  if ((UINTN)MemoryDataLocation != mXtuSmiBaseAddress) {
    (pCpuSaveState->x64)._RBX = (UINT32)0x00008007;  // Memory Location does not match
    return Status;
  }
#endif
//AMI_OVERRIDE_END - Override for AMI SMM function to access CPU register

  TurboModeSupported         = isTurboModeSupported();
  TurboTdpLimitProgrammable  = isXETdcTdpLimitSupported();
  CoreRatioLimitAvailable    = isCoreRatioLimitSupported();

  if ((ReadWrite != OVERCLOCK_SMI_READ_COMMAND) && (ReadWrite != OVERCLOCK_SMI_WRITE_COMMAND) &&
     (ReadWrite != OVERCLOCK_SMI_WDT_CONFIRM_REQUEST_COMMAND)) {
//AMI_OVERRIDE_START - Override for AMI SMM to access CPU register
/*
    (pCpuSaveState->x64)._RBX = (UINT32)0x00008004;  //Command not valid in ECX
*/	
    ErrorCode = 0x00008004;
	Status = gSmmCpu->WriteSaveState ( gSmmCpu, \
	                                         4, \
	                                         EFI_SMM_SAVE_STATE_REGISTER_RBX, \
	                                         Cpu, \
	                                         &ErrorCode );
//AMI_OVERRIDE_END - Override for AMI SMM to access CPU register
    return Status;
  }

  //
  // Read BIOS settings data from Setup and pass it to OS Application through MemoryDataLocation
  //
  if (ReadWrite == OVERCLOCK_SMI_READ_COMMAND) {
    UINT16 XtuCoreVoltageOffset;
    UINT8  XtuOcEnable;
//AMI_OVERRIDE_START - Fix EIP229470	
    UINT16 XtuDdrRatioValue;
//AMI_OVERRIDE_END - Fix EIP229470	
    UINT16 XtuGtVoltageOffset;
    UINT16 XtuUncoreVoltageOffset;

    // HeaderLength = Signature + Length + MajorRev + MinorRev
    HeaderLength = sizeof(BIOS_SETTINGS_DATA_HEADER) - sizeof(UINT32);

    // Total size of BIOS Setting Data Structure = Header + Size of Static Entries + Size of Dynamic Entries
    // Static entries = entries added without condtional logic.
    // Dynamic etries = entries added based on conditional logic.
    Length = sizeof(BIOS_SETTINGS_DATA_HEADER) + (NO_OF_BIOS_SETTING_STATIC_ENTRIES*sizeof(BIOS_SETTING_ENTRY));
    Count = NO_OF_BIOS_SETTING_STATIC_ENTRIES;

    //
    // Now add the sizes of the conditional control ID's
    //
    if ( TurboModeSupported ) {
      // one entry for Turbo Mode Enable/Disable
      Length += sizeof(BIOS_SETTING_ENTRY);
      Count++;
      // entries for Long ,Short Duration Power Limit Enable/Disable, Short Duration Turbo Power Limit and Turbo Power Limit Lock
      if( TurboTdpLimitProgrammable ) {
        Length += (4*sizeof(BIOS_SETTING_ENTRY));
        Count += 4;
      }
      // 4 entries for Turbo Core Ratio's
      if ( CoreRatioLimitAvailable ) {
        //
        // Entries for 1C and 2C
        //
        Length += (2*sizeof(BIOS_SETTING_ENTRY));
        Count += 2;
        //
        // Entries for 3C and 4C
        //
        if( mNumberOfActiveCores > 2 ) {
          Length += (2*sizeof(BIOS_SETTING_ENTRY));
          Count += 2;
        }
        //
        // Entries for 5C and 6C
        //
        if( mNumberOfActiveCores > 4 ) {
          Length += (2*sizeof(BIOS_SETTING_ENTRY));
          Count += 2;
        }
        //
        // Entries for 7C and 8C
        //
        if( mNumberOfActiveCores > 6 ) {
          Length += (2*sizeof(BIOS_SETTING_ENTRY));
          Count += 2;
        }
      }
    }
//AMI_OVERRIDE_START - Overclocking part needs to be updated for SA50011 [ EIP437916 ]
    Status = AmiValidateMemoryBuffer((VOID*) MemoryDataLocation, sizeof(BIOS_SETTINGS_DATA_HEADER));
    if (EFI_ERROR(Status)) return EFI_SECURITY_VIOLATION;
//AMI_OVERRIDE_END - Overclocking part needs to be updated for SA50011 [ EIP437916 ]
    if (MemoryDataLocation->Signature != BIOSSETTINGS_SIG_REV4) {
      if (MemoryDataLocation->Signature != BIOSSETTINGS_SIG_REV3) {
        //return error code EBX
//AMI_OVERRIDE_START - Override for AMI SMM to access CPU register
/*
        (pCpuSaveState->x64)._RBX = (UINT32)0x00008001;  //signature wrong
*/
    	ErrorCode = 0x00008001;
    	Status = gSmmCpu->WriteSaveState ( gSmmCpu, \
    	                                   4, \
    	                                   EFI_SMM_SAVE_STATE_REGISTER_RBX, \
    	                                   Cpu, \
    	                                   &ErrorCode );
//AMI_OVERRIDE_END - Override for AMI SMM to access CPU register
        return Status;
      } else {
        //update Sign, length, MajorRev, MinorRev.
        MemoryDataLocation->Signature = BIOSSETTINGS_SIG_REV4;
        MemoryDataLocation->Length = Length;
        MemoryDataLocation->MajorRev = 2;
        MemoryDataLocation->MinorRev = 0;
//AMI_OVERRIDE_START - Override for AMI SMM to access CPU register
/*
        (pCpuSaveState->x64)._RBX = (UINT32)0x00000001;  // Warning Old Signature $BD$
*/
    	ErrorCode = 0x00000001;
    	Status = gSmmCpu->WriteSaveState ( gSmmCpu, \
    	                                   4, \
    	                                   EFI_SMM_SAVE_STATE_REGISTER_RBX, \
    	                                   Cpu, \
    	                                   &ErrorCode );
//AMI_OVERRIDE_END - Override for AMI SMM to access CPU register
        return Status;
      }
    } else if(MemoryDataLocation->Length > XTU_BUFFER_LENGTH) {
//AMI_OVERRIDE_START - Fix one building error in OC module after updating RC 1.0.1.
/*
      (pCpuSaveState->x64)._RBX = (UINT32)0x00008008;  // Table length too large, no header data returned.
*/
	  ErrorCode = 0x00008008;
	  Status = gSmmCpu->WriteSaveState ( gSmmCpu, \
	    	                                4, \
	    	                                EFI_SMM_SAVE_STATE_REGISTER_RBX, \
	    	                                Cpu, \
	    	                                &ErrorCode );
//AMI_OVERRIDE_END - Fix one building error in OC module after updating RC 1.0.1.
      return Status;
    } else if(MemoryDataLocation->Length > Length) {
        MemoryDataLocation->Length = Length;
        MemoryDataLocation->MajorRev = 2;
        MemoryDataLocation->MinorRev = 0;
//AMI_OVERRIDE_START - Override for AMI SMM to access CPU register
/*
      (pCpuSaveState->x64)._RBX = (UINT32)0x00000002;  // length larger than expected
*/
    	ErrorCode = 0x00000002;
    	Status = gSmmCpu->WriteSaveState ( gSmmCpu, \
    	                                   4, \
    	                                   EFI_SMM_SAVE_STATE_REGISTER_RBX, \
    	                                   Cpu, \
    	                                   &ErrorCode );
//AMI_OVERRIDE_END - Override for AMI SMM to access CPU register
    } else if(MemoryDataLocation->Length < HeaderLength) {
//AMI_OVERRIDE_START - Override for AMI SMM to access CPU register
/*
      (pCpuSaveState->x64)._RBX = (UINT32)0x00008003;  // Table length too small, no header data returned.
*/
    	ErrorCode = 0x00008003;
    	Status = gSmmCpu->WriteSaveState ( gSmmCpu, \
    	                                   4, \
    	                                   EFI_SMM_SAVE_STATE_REGISTER_RBX, \
    	                                   Cpu, \
    	                                   &ErrorCode );
//AMI_OVERRIDE_END - Override for AMI SMM to access CPU register
      return Status;
    } else if(MemoryDataLocation->Length < Length) {
      MemoryDataLocation->Length = Length;
      MemoryDataLocation->MajorRev = 2;
      MemoryDataLocation->MinorRev = 0;
//AMI_OVERRIDE_START - Override for AMI SMM to access CPU register
/*
      (pCpuSaveState->x64)._RBX = (UINT32)0x00008002;  // Table length too small, valid header data returned.
*/	  
    	ErrorCode = 0x00008002;
    	Status = gSmmCpu->WriteSaveState ( gSmmCpu, \
    	                                   4, \
    	                                   EFI_SMM_SAVE_STATE_REGISTER_RBX, \
    	                                   Cpu, \
    	                                   &ErrorCode );
//AMI_OVERRIDE_END - Override for AMI SMM to access CPU register
      return Status;
    } else {
//AMI_OVERRIDE_START - Override for AMI SMM to access CPU register
/*
      (pCpuSaveState->x64)._RBX = (UINT32)0x0;  //successful
*/		
    	ErrorCode = 0x0;
    	Status = gSmmCpu->WriteSaveState ( gSmmCpu, \
    	                                   4, \
    	                                   EFI_SMM_SAVE_STATE_REGISTER_RBX, \
    	                                   Cpu, \
    	                                   &ErrorCode );
//AMI_OVERRIDE_END - Override for AMI SMM to access CPU register
      MemoryDataLocation->MajorRev = 2;
      MemoryDataLocation->MinorRev = 0;
    }
//AMI_OVERRIDE_START - Overclocking part needs to be updated for SA50011 [ EIP437916 ]
    Status = AmiValidateMemoryBuffer((VOID*) MemoryDataLocation, (UINTN)MemoryDataLocation->Length);
    if (EFI_ERROR(Status)) return EFI_SECURITY_VIOLATION;
//AMI_OVERRIDE_END - Overclocking part needs to be updated for SA50011 [ EIP437916 ]
    DEBUG ((DEBUG_INFO, "OC SMI: SMI entry Count = %X\n", Count));
    MemoryDataLocation->Count = Count;

    //
    // EIST
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.EnableGv, BIOS_DEVICE_EIST);

    //
    // Flex ratio
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.CpuRatio, BIOS_DEVICE_FLEX_RATIO);

    //
    // CAS Latency
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.tCL, BIOS_DEVICE_tCL);

    //
    // tRCD / tRP (same value on SKL)
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.tRCDtRP, BIOS_DEVICE_tRCD_tRP);

    //
    // tRAS
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.tRAS, BIOS_DEVICE_tRAS);

    //
    // tWR
    //
//AMI_OVERRIDE_START
#if 0
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.tWR, BIOS_DEVICE_tWR);
#else
    ADD_ENTRY_TO_BIOS_SETTING_TABLE_AUTOMATIC (BiosSettingEntries, Index, OcSetupData.tWR, BIOS_DEVICE_tWR, 0);
#endif	
//AMI_OVERRIDE_END

    //
    // tRFC
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.tRFC, BIOS_DEVICE_tRFC);

    //
    // tRRD
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.tRRD, BIOS_DEVICE_tRRD);

    //
    // tWTR
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.tWTR, BIOS_DEVICE_tWTR);

    //
    // tRTP
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.tRTP, BIOS_DEVICE_tRTP);

    //
    // tFAW
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.tFAW, BIOS_DEVICE_tFAW);

    //
    // System Command Rate (NMode)
    //
//AMI_OVERRIDE_START
#if 0
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.NModeSupport, BIOS_DEVICE_SYSTEM_CMD_RATE);
#else
    ADD_ENTRY_TO_BIOS_SETTING_TABLE_AUTOMATIC (BiosSettingEntries, Index, OcSetupData.NModeSupport, BIOS_DEVICE_SYSTEM_CMD_RATE, 0);
#endif	
//AMI_OVERRIDE_END

    //
    // DDR Multiplier
    //
//AMI_OVERRIDE_START - Fix EIP229470
#if 0
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.DdrRatio, BIOS_DEVICE_DDR_MULTIPLIER);
#else
    XtuDdrRatioValue = OcSetupData.DdrFreqLimit;
    ADD_ENTRY_TO_BIOS_SETTING_TABLE_AUTOMATIC (BiosSettingEntries, Index, XtuDdrRatioValue, BIOS_DEVICE_DDR_MULTIPLIER, 0x0);
#endif
//AMI_OVERRIDE_END - Fix EIP229470

    //
    // Memory Voltage
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.MemoryVoltage, BIOS_DEVICE_MEMORY_VOLTAGE);

    //
    // Host Clock Frequency entry
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.BclkFrequency, BIOS_DEVICE_HOST_CLK_FREQ);

    //
    // Internal Graphics Freq
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.GtsMaxOcRatio, BIOS_DEVICE_IGFX_FREQUENCY);

    //
    // Internal Graphics UnSliced Freq
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.GtusMaxOcRatio, BIOS_DEVICE_IGFX_UNSLICED_FREQUENCY);

    //
    // Core Voltage Mode
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.CoreVoltageMode, BIOS_DEVICE_IA_CORE_VOLTAGE_MODE);

    //
    // ICC MAX
    //
#if 0 // AMI_OVERRIDE_START
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.IccMax[0], BIOS_DEVICE_SA_CURRENT_LIMIT_MAX);
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.IccMax[1], BIOS_DEVICE_IA_CURRENT_LIMIT_MAX);
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.IccMax[2], BIOS_DEVICE_GTUS_CURRENT_LIMIT_MAX);
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.IccMax[3], BIOS_DEVICE_GTS_CURRENT_LIMIT_MAX);

#else
    if (PlatformFlavor == FlavorMobile) {
        ADD_ENTRY_TO_BIOS_SETTING_TABLE_AUTOMATIC (BiosSettingEntries, Index , OcSetupData.IccMax[0], BIOS_DEVICE_SA_CURRENT_LIMIT_MAX, 0x0);
    }
    
    ADD_ENTRY_TO_BIOS_SETTING_TABLE_AUTOMATIC (BiosSettingEntries, Index , OcSetupData.IccMax[1], BIOS_DEVICE_IA_CURRENT_LIMIT_MAX, 0x0);
    
    ADD_ENTRY_TO_BIOS_SETTING_TABLE_AUTOMATIC (BiosSettingEntries, Index , OcSetupData.IccMax[2], BIOS_DEVICE_GTUS_CURRENT_LIMIT_MAX, 0x0);
    
    ADD_ENTRY_TO_BIOS_SETTING_TABLE_AUTOMATIC (BiosSettingEntries, Index , OcSetupData.IccMax[3], BIOS_DEVICE_GTS_CURRENT_LIMIT_MAX, 0x0);
#endif // AMI_OVERRIDE_END
    //
    // AVX Ratio control
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.Avx2RatioOffset, BIOS_DEVICE_IA_AVX2_CORE_RATIO_OFFSET);

    //
    // Core Voltage
    //
    if (OcSetupData.CoreVoltageMode == VOLTAGE_MODE_ADAPTIVE) {
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.CoreExtraTurboVoltage, BIOS_DEVICE_CPU_VOLTAGE);
    }
    else if (OcSetupData.CoreVoltageMode == VOLTAGE_MODE_OVERRIDE) {
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.CoreVoltageOverride, BIOS_DEVICE_CPU_VOLTAGE);
    }
    else {
      DEBUG ((DEBUG_INFO, "OC SMI: Core Voltage Mode invalid: %X\n", OcSetupData.CoreVoltageMode));
    }

    //
    // Core Voltage Offset
    //
    //
    // Need to translate to voltage offset representation for XTU
    //
    if (OcSetupData.CoreVoltageOffsetPrefix == 1) {
      //
      // Offset is negative, convert to XTU representation where -1000 to 0 is 0 to 1000
      //
      XtuCoreVoltageOffset = 1000 - OcSetupData.CoreVoltageOffset;
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , XtuCoreVoltageOffset, BIOS_DEVICE_IA_CORE_VOLTAGE_OFFSET);
    }
    else {
      //
      // Number is postive, convert to XTU representation where 1 to 1000 is 1001 to 2000
      //
      XtuCoreVoltageOffset = OcSetupData.CoreVoltageOffset + 1000;
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , XtuCoreVoltageOffset, BIOS_DEVICE_IA_CORE_VOLTAGE_OFFSET);
    }

    //
    // Turbo Boost Power Time Window
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.PowerLimit1Time, BIOS_DEVICE_TURBO_BOOST_POWER_TIME_WINDOWS);

    //
    // XMP Profile
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.SpdProfileSelected, BIOS_DEVICE_XMP_PROFILE);

    //
    // Memory Clock Multiplier
    //
//AMI_OVERRIDE_START - Fix SetupData error DdrRefClk: 1 is 133 and 2 is 100, but OC 0 is 100, 1 is 133
#if 0
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.DdrRefClk, BIOS_DEVICE_MEMORY_CLOCK_FREQUENCY);
#else
    if (OcSetupData.DdrRefClk == 1) {
        ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , 1, BIOS_DEVICE_MEMORY_CLOCK_FREQUENCY);
    } else {
        ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , 0, BIOS_DEVICE_MEMORY_CLOCK_FREQUENCY);
    }
#endif
//AMI_OVERRIDE_END - Fix SetupData error DdrRefClk: 1 is 133 and 2 is 100, but OC 0 is 100, 1 is 133

    //
    // Turbo Mode
    //
    if ( TurboModeSupported ) {
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.TurboMode, BIOS_DEVICE_TURBO_MODE);

       if ( CoreRatioLimitAvailable ) {
         ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.RatioLimit1, BIOS_DEVICE_1_CORE_RATIO_LIMIT);

         ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.RatioLimit2, BIOS_DEVICE_2_CORE_RATIO_LIMIT);

         if ( mNumberOfActiveCores > 2 ) {
           ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.RatioLimit3, BIOS_DEVICE_3_CORE_RATIO_LIMIT);

           ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.RatioLimit4, BIOS_DEVICE_4_CORE_RATIO_LIMIT);

           if ( mNumberOfActiveCores > 4 ) {
             ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.RatioLimit5, BIOS_DEVICE_5_CORE_RATIO_LIMIT);

             ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.RatioLimit6, BIOS_DEVICE_6_CORE_RATIO_LIMIT);

             if ( mNumberOfActiveCores > 6 ) {
               ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.RatioLimit7, BIOS_DEVICE_7_CORE_RATIO_LIMIT);

               ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.RatioLimit8, BIOS_DEVICE_8_CORE_RATIO_LIMIT);
             }
           }
         }
       }

      if ( TurboTdpLimitProgrammable ) {
        DEBUG ((DEBUG_INFO, "OC SMI: TurboTdpLimitProgrammable, creating bios setting table\n"));

        ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.TurboPowerLimitLock, BIOS_DEVICE_TURBO_POWER_LIMIT_LOCK);

        ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.PowerLimit2, BIOS_DEVICE_SHORT_WINDOW_POWER_ENABLE);

// AMI_OVERRIDE_START - Fix that Turbo power and power limitation don't sync with setup value.
#if 0        
        ADD_ENTRY_TO_BIOS_SETTING_TABLE_AUTOMATIC (BiosSettingEntries, Index , OcSetupData.PowerLimit2Power, BIOS_DEVICE_SHORT_WINDOW_TDP_LIMIT, OcSetupData.PowerLimit2Default);

        ADD_ENTRY_TO_BIOS_SETTING_TABLE_AUTOMATIC (BiosSettingEntries, Index , OcSetupData.PowerLimit1, BIOS_DEVICE_EXTENDED_TDP_LIMIT, OcSetupData.PowerLimit1Default);
#else        
        ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.PowerLimit2Default / 125, BIOS_DEVICE_SHORT_WINDOW_TDP_LIMIT);
        
        ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.PowerLimit1Default / 125, BIOS_DEVICE_EXTENDED_TDP_LIMIT);
#endif        
// AMI_OVERRIDE_END - Fix that Turbo power and power limitation don't sync with setup value.        
      }
    }

    //
    // Ring Ratio
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.RingMaxOcRatio, BIOS_DEVICE_RING_RATIO);

    //
    // Overclocking Enable
    //

    //
    // XTU definition of OC Enable is inverted from BIOS defintion of OC Lock.
    // For Read commands, we need to populate this value correctly. Write command
    // is don't care since we will do the write handling further down in the function.
    //
    XtuOcEnable = (UINT8) ~OcSetupData.OverclockingLock & BIT0;
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , XtuOcEnable, BIOS_DEVICE_OC_ENABLE);

    ///
    /// GT Slide
    ///
    //
    // GT Voltage Mode
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.GtsVoltageMode, BIOS_DEVICE_IGFX_VOLTAGE_MODE);

    //
    // GT Voltage Override
    //
    if (OcSetupData.GtsVoltageMode == VOLTAGE_MODE_ADAPTIVE) {
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.GtsExtraTurboVoltage, BIOS_DEVICE_IGFX_VOLTAGE_OVERRIDE);
    }
    else if (OcSetupData.GtsVoltageMode == VOLTAGE_MODE_OVERRIDE) {
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.GtsVoltageOverride, BIOS_DEVICE_IGFX_VOLTAGE_OVERRIDE);
    }
    else {
      DEBUG ((DEBUG_INFO, "OC SMI: GT Voltage Mode invalid: %X\n", OcSetupData.GtsVoltageMode));
    }

    //
    // GT Voltage Offset
    //

    //
    // Need to translate to voltage offset representation for XTU
    //
    if (OcSetupData.GtsVoltageOffsetPrefix == 1) {
      //
      // Offset is negative, convert to XTU representation where -1000 to 0 is 0 to 1000
      //
      XtuGtVoltageOffset = 1000 - OcSetupData.GtsVoltageOffset;
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , XtuGtVoltageOffset, BIOS_DEVICE_IGFX_VOLTAGE_OFFSET);
    }
    else {
      //
      // Number is postive, convert to XTU representation where 1 to 1000 is 1001 to 2000
      //
      XtuGtVoltageOffset = OcSetupData.GtsVoltageOffset + 1000;
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , XtuGtVoltageOffset, BIOS_DEVICE_IGFX_VOLTAGE_OFFSET);
    }

    ///
    /// GT UnSliced
    ///
    //
    // GT Voltage Mode
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.GtusVoltageMode, BIOS_DEVICE_IGFX_UNSLICED_VOLTAGE_MODE);

    //
    // GT Voltage Override
    //
    if (OcSetupData.GtusVoltageMode == VOLTAGE_MODE_ADAPTIVE) {
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.GtusExtraTurboVoltage, BIOS_DEVICE_IGFX_UNSLICED_VOLTAGE_OVERRIDE);
    }
    else if (OcSetupData.GtusVoltageMode == VOLTAGE_MODE_OVERRIDE) {
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , OcSetupData.GtusVoltageOverride, BIOS_DEVICE_IGFX_UNSLICED_VOLTAGE_OVERRIDE);
    }
    else {
      DEBUG ((DEBUG_INFO, "OC SMI: GT UnSliced Voltage Mode invalid: %X\n", OcSetupData.GtusVoltageMode));
    }

    //
    // GT Voltage Offset
    //

    //
    // Need to translate to voltage offset representation for XTU
    //
    if (OcSetupData.GtusVoltageOffsetPrefix == 1) {
      //
      // Offset is negative, convert to XTU representation where -1000 to 0 is 0 to 1000
      //
      XtuGtVoltageOffset = 1000 - OcSetupData.GtusVoltageOffset;
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , XtuGtVoltageOffset, BIOS_DEVICE_IGFX_UNSLICED_VOLTAGE_OFFSET);
    }
    else {
      //
      // Number is postive, convert to XTU representation where 1 to 1000 is 1001 to 2000
      //
      XtuGtVoltageOffset = OcSetupData.GtusVoltageOffset + 1000;
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , XtuGtVoltageOffset, BIOS_DEVICE_IGFX_UNSLICED_VOLTAGE_OFFSET);
    }

    //
    // SA Voltage Offset
    //

    //
    // Need to translate to voltage offset representation for XTU
    //
    if (OcSetupData.UncoreVoltageOffsetPrefix == 1) {
      //
      // Offset is negative, convert to XTU representation where -1000 to 0 is 0 to 1000
      //
      XtuUncoreVoltageOffset = 1000 - OcSetupData.UncoreVoltageOffset;
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , XtuUncoreVoltageOffset, BIOS_DEVICE_SA_VOLTAGE_OFFSET);
    }
    else {
      //
      // Number is postive, convert to XTU representation where 1 to 1000 is 1001 to 2000
      //
      XtuUncoreVoltageOffset = OcSetupData.UncoreVoltageOffset + 1000;
      ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index , XtuUncoreVoltageOffset, BIOS_DEVICE_SA_VOLTAGE_OFFSET);
    }

    //
    // RealTime Memory Timing Enable/Disable
    //
    ADD_ENTRY_TO_BIOS_SETTING_TABLE (BiosSettingEntries, Index, OcSetupData.RealtimeMemoryTiming, BIOS_DEVICE_REAL_TIME_MEMORY_TIMING_ENABLE);



    DEBUG ((DEBUG_INFO, "OC SMI: BIOS Settings Index = %X\n", Index));
  }

  //
  // Write BIOS settings data from MemoryDataLocation to BIOS Setup data
  //
  if (ReadWrite == OVERCLOCK_SMI_WRITE_COMMAND) {
//AMI_OVERRIDE_START - Overclocking part needs to be updated for SA50011 [ EIP437916 ]
    if (MemoryDataLocation->Signature != BIOSSETTINGS_SIG_REV4) {
        return EFI_INVALID_PARAMETER;
    }
    if (MemoryDataLocation->Length == 0) {
        return EFI_INVALID_PARAMETER;
    }
    if (MemoryDataLocation->Length < (UINT32)(MemoryDataLocation->Count * sizeof(BIOS_SETTING_ENTRY) + sizeof(BIOS_SETTINGS_DATA_HEADER))) {
        return EFI_INVALID_PARAMETER;
    }
    Status = AmiValidateMemoryBuffer((VOID*) MemoryDataLocation, (UINTN)MemoryDataLocation->Length);
    if(EFI_ERROR(Status)) return EFI_SECURITY_VIOLATION;
//AMI_OVERRIDE_END - Overclocking part needs to be updated for SA50011 [ EIP437916 ]
    OcSetupData.OverclockingSupport = ENABLE_SUPPRORT;

    Count = MemoryDataLocation->Count;

    //
    //  Ensure we do not exceed the XTU buffer length
    //
    if (MemoryDataLocation->Length > XTU_BUFFER_LENGTH) {
      MaxEntries = XTU_BUFFER_LENGTH / sizeof(BIOS_SETTING_ENTRY);
    } else {
      MaxEntries = (UINT32)((MemoryDataLocation->Length - HeaderLength) / sizeof(BIOS_SETTING_ENTRY));
    }

    if (Count > MaxEntries) {
      Count = MaxEntries;
    }

    for ( Index = 0; Index < Count; Index++ ) {
      BiosSettingValue = BiosSettingEntries[Index].SettingValue;

      switch ( BiosSettingEntries[Index].BiosImplementationType ) {

        case BIOS_DEVICE_FLEX_RATIO:
           if (OcSetupData.CpuRatio != (UINT8) BiosSettingValue) {
             //
             // Enable Flex Overclock Support, if Non Turbo Flex Ratio input parameters are
             // changed from Setup data.
             //
             OcSetupData.CpuRatioOverride = ENABLE_SUPPRORT;
             OcSetupData.CpuRatio  = (UINT8) BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_HOST_CLK_FREQ:
           if (OcSetupData.BclkFrequency != (UINT32) BiosSettingValue) {
             OcSetupData.BclkFrequency  = (UINT32) BiosSettingValue;
             OcSetupData.BclkOverride   = 1;
           }
           break;

        case BIOS_DEVICE_tCL:
           if (OcSetupData.tCL != (UINT8) BiosSettingValue) {
             OcSetupData.tCL = (UINT8) BiosSettingValue;
             OcSetupData.SpdProfileSelected = SPD_PROFILE_CUSTOM;
           }
           break;

        case BIOS_DEVICE_tRCD_tRP:
           if (OcSetupData.tRCDtRP != (UINT8) BiosSettingValue) {
             OcSetupData.tRCDtRP = (UINT8) BiosSettingValue;
             OcSetupData.SpdProfileSelected = SPD_PROFILE_CUSTOM;
           }
           break;

        case BIOS_DEVICE_tRAS:
           if (OcSetupData.tRAS != (UINT16) BiosSettingValue) {
             OcSetupData.tRAS  = (UINT16) BiosSettingValue;
             OcSetupData.SpdProfileSelected = SPD_PROFILE_CUSTOM;
           }
           break;

        case BIOS_DEVICE_tWR:
#if 0 //AMI_OVERRIDE_START
           if (OcSetupData.tWR != (UINT8) BiosSettingValue) {
#else
           if (BiosSettingValue == AUTOMATIC_MODE) {
             OcSetupData.tWR = OcSetupData.tWRDefault;
           } else if (OcSetupData.tWR != (UINT8) BiosSettingValue) {
#endif //AMI_OVERRIDE_END
             OcSetupData.tWR = (UINT8) BiosSettingValue;
             OcSetupData.SpdProfileSelected = SPD_PROFILE_CUSTOM;
           }
           break;

        case BIOS_DEVICE_tRFC:
           if (OcSetupData.tRFC != (UINT16) BiosSettingValue) {
             OcSetupData.tRFC = (UINT16) BiosSettingValue;
             OcSetupData.SpdProfileSelected = SPD_PROFILE_CUSTOM;
           }
           break;

        case BIOS_DEVICE_tRRD:
           if (OcSetupData.tRRD != (UINT8) BiosSettingValue) {
             OcSetupData.tRRD = (UINT8) BiosSettingValue;
             OcSetupData.SpdProfileSelected = SPD_PROFILE_CUSTOM;
           }
           break;

        case BIOS_DEVICE_tWTR:
           if (OcSetupData.tWTR != (UINT8) BiosSettingValue) {
             OcSetupData.tWTR = (UINT8) BiosSettingValue;
             OcSetupData.SpdProfileSelected = SPD_PROFILE_CUSTOM;
           }
           break;

        case BIOS_DEVICE_tRTP:
           if (OcSetupData.tRTP != (UINT8) BiosSettingValue) {
             OcSetupData.tRTP = (UINT8) BiosSettingValue;
             OcSetupData.SpdProfileSelected = SPD_PROFILE_CUSTOM;
           }
           break;

        case BIOS_DEVICE_tFAW:
           if (OcSetupData.tFAW != (UINT16) BiosSettingValue) {
             OcSetupData.tFAW  = (UINT16) BiosSettingValue;
             OcSetupData.SpdProfileSelected = SPD_PROFILE_CUSTOM;
           }
           break;

        case BIOS_DEVICE_SYSTEM_CMD_RATE:
#if 0 //AMI_OVERRIDE_START
           if (OcSetupData.NModeSupport != (UINT8) BiosSettingValue) {
#else
           if (BiosSettingValue == AUTOMATIC_MODE) {
             OcSetupData.NModeSupport = 0;
           } else if (OcSetupData.NModeSupport != (UINT8) BiosSettingValue) {
#endif //AMI_OVERRIDE_END
             OcSetupData.NModeSupport = (UINT8) BiosSettingValue;
             OcSetupData.SpdProfileSelected = SPD_PROFILE_CUSTOM;
           }
           break;

        case BIOS_DEVICE_MEMORY_VOLTAGE:
           if (OcSetupData.MemoryVoltage != (UINT16) BiosSettingValue) {
             OcSetupData.MemoryVoltage = (UINT16) BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_DDR_MULTIPLIER:
#if 0 //AMI_OVERRIDE_START - Fix EIP229470
           if (OcSetupData.DdrRatio != (UINT8) BiosSettingValue) {
             OcSetupData.DdrRatio = (UINT8) BiosSettingValue;
           }
#else
           if ((BiosSettingValue == AUTOMATIC_MODE) && (OcSetupData.DdrFreqLimit == 0)) {
             OcSetupData.DdrRatio = DISABLE_SUPPRORT;
             OcSetupData.DdrFreqLimit = (UINT16) BiosSettingValue;
             OcSetupData.DdrOddRatioMode = DDR_ODD_RATIO_MODE_DISABLE;
           }else if (BiosSettingValue % 100) { // DdrRefClk is 133
               DdrRatioValue = ((BiosSettingValue * 3)/ 4)/ 100;
               OcSetupData.DdrFreqLimit = (UINT16) BiosSettingValue;
        	   if (DdrRatioValue % 2) {  // DdrOddRatioMode is enabled
        	       OcSetupData.DdrOddRatioMode = DDR_ODD_RATIO_MODE_ENABLE;
        	       OcSetupData.DdrRatio = (UINT8) ((DdrRatioValue - 1)/ 2);
        	   }else {  // DdrOddRatioMode is disabled
        	       OcSetupData.DdrRatio = (UINT8) (DdrRatioValue / 2);
        	       OcSetupData.DdrOddRatioMode = DDR_ODD_RATIO_MODE_DISABLE;
        	   }
           }else { // DdrRefClk is 100
               DdrRatioValue = BiosSettingValue / 100;
               OcSetupData.DdrFreqLimit = (UINT16) BiosSettingValue;
               if (DdrRatioValue % 2) { // DdrOddRatioMode is enabled
        	       OcSetupData.DdrRatio = (UINT8) ((DdrRatioValue - 1)/ 2);
        	       OcSetupData.DdrOddRatioMode = DDR_ODD_RATIO_MODE_ENABLE;
               }else { // DdrOddRatioMode is disabled
        	       OcSetupData.DdrRatio = (UINT8) (DdrRatioValue / 2);
        	       OcSetupData.DdrOddRatioMode = DDR_ODD_RATIO_MODE_DISABLE;
               }
           }
#endif //AMI_OVERRIDE_END - Fix EIP229470
           break;

        case BIOS_DEVICE_TURBO_MODE:
           if (OcSetupData.TurboMode != (UINT8) BiosSettingValue ) {
             OcSetupData.TurboMode = (UINT8)BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_EIST:
           if (OcSetupData.EnableGv != (UINT8) BiosSettingValue ) {
             OcSetupData.EnableGv = (UINT8)BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_1_CORE_RATIO_LIMIT:
           if (OcSetupData.RatioLimit1  != (UINT8) BiosSettingValue) {
             OcSetupData.RatioLimit1     = (UINT8) BiosSettingValue;
             OcSetupData.OverclockingSupport = 1;
             // We need to sync the 1 Core ratio limit with the OC mailbox
             // core ratio limit since the mailbox interface will also limit
             // the core CPU frequency. Without this sync, the core ratios
             // may not achieve the desired frequency.
             OcSetupData.CoreMaxOcRatio  = (UINT8) BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_2_CORE_RATIO_LIMIT:
           if (OcSetupData.RatioLimit2  != (UINT8) BiosSettingValue) {
             OcSetupData.RatioLimit2 = (UINT8) BiosSettingValue;
             OcSetupData.OverclockingSupport = 1;
           }
           break;

        case BIOS_DEVICE_3_CORE_RATIO_LIMIT:
           if (OcSetupData.RatioLimit3 != (UINT8) BiosSettingValue) {
             OcSetupData.RatioLimit3 = (UINT8) BiosSettingValue;
             OcSetupData.OverclockingSupport = 1;
           }
           break;

        case BIOS_DEVICE_4_CORE_RATIO_LIMIT:
           if (OcSetupData.RatioLimit4  != (UINT8) BiosSettingValue) {
             OcSetupData.RatioLimit4 = (UINT8) BiosSettingValue;
             OcSetupData.OverclockingSupport = 1;
           }
           break;

        case BIOS_DEVICE_5_CORE_RATIO_LIMIT:
           if (OcSetupData.RatioLimit5  != (UINT8) BiosSettingValue) {
             OcSetupData.RatioLimit5 = (UINT8) BiosSettingValue;
             OcSetupData.OverclockingSupport = 1;
           }
           break;

        case BIOS_DEVICE_6_CORE_RATIO_LIMIT:
           if (OcSetupData.RatioLimit6 != (UINT8) BiosSettingValue) {
             OcSetupData.RatioLimit6 = (UINT8) BiosSettingValue;
             OcSetupData.OverclockingSupport = 1;
           }
           break;

        case BIOS_DEVICE_7_CORE_RATIO_LIMIT:
           if (OcSetupData.RatioLimit7  != (UINT8) BiosSettingValue) {
             OcSetupData.RatioLimit7 = (UINT8) BiosSettingValue;
             OcSetupData.OverclockingSupport = 1;
           }
           break;

        case BIOS_DEVICE_8_CORE_RATIO_LIMIT:
           if (OcSetupData.RatioLimit8  != (UINT8) BiosSettingValue) {
             OcSetupData.RatioLimit8 = (UINT8) BiosSettingValue;
             OcSetupData.OverclockingSupport = 1;
           }
           break;

        case BIOS_DEVICE_SHORT_WINDOW_POWER_ENABLE:
           if (OcSetupData.PowerLimit2 != (UINT8) BiosSettingValue) {
             OcSetupData.PowerLimit2 = (UINT8) BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_SHORT_WINDOW_TDP_LIMIT:
           //
           //  XTU has power limits defined as Watts, whereas BIOS stores as milliwatts.
           //  Need to convert XTU value to milliwatts before updating SetupSa
           //
// AMI_OVERRIDE_START - Fix that Turbo power and power limitation don't sync with setup value.           
#if 0            
           if (BiosSettingValue == AUTOMATIC_MODE) {
             OcSetupData.PowerLimit2Power = OcSetupData.PowerLimit2Default;
           } else if (OcSetupData.PowerLimit2Power != ((UINT32) BiosSettingValue)) {
           OcSetupData.PowerLimit2Power  = ((UINT32) BiosSettingValue);
           }
#else
           if (OcSetupData.PowerLimit2Power != ((UINT32) BiosSettingValue * 125)) {
             OcSetupData.PowerLimit2Power  = ((UINT32) BiosSettingValue * 125);
           }           
#endif
// AMI_OVERRIDE_END - Fix that Turbo power and power limitation don't sync with setup value.
           break;

        case BIOS_DEVICE_EXTENDED_TDP_LIMIT:
           //
           //  XTU has power limits defined as Watts, whereas BIOS stores as milliwatts.
           //  Need to convert XTU value to milliwatts before updating SetupSa
           //
// AMI_OVERRIDE_START - Fix that Turbo power and power limitation don't sync with setup value and waring messages in BET tool.
#if 0            
           if (BiosSettingValue == AUTOMATIC_MODE) {
             OcSetupData.LongDurationPwrLimitOverride = 0;
           } else if (OcSetupData.PowerLimit1 != ((UINT32) BiosSettingValue)) {
             OcSetupData.PowerLimit1 = ((UINT32) BiosSettingValue);
             OcSetupData.LongDurationPwrLimitOverride = 1;
           }
#else
           if (OcSetupData.PowerLimit1Default == ((UINT32) BiosSettingValue * 125)) {
             OcSetupData.PowerLimit1 = OcSetupData.PowerLimit1Default;
             OcSetupData.LongDurationPwrLimitOverride = 0;
           } else if (OcSetupData.PowerLimit1 != ((UINT32) BiosSettingValue * 125)) {
             OcSetupData.PowerLimit1 = ((UINT32) BiosSettingValue * 125);
             OcSetupData.LongDurationPwrLimitOverride = 1;
           }           
#endif
// AMI_OVERRIDE_END - Fix that Turbo power and power limitation don't sync with setup value and waring messages in BET tool.
           break;

        case BIOS_DEVICE_TURBO_POWER_LIMIT_LOCK:
           if (BiosSettingValue == AUTOMATIC_MODE) {
             OcSetupData.TurboPowerLimitLock = DISABLE_SUPPRORT;
           } else if (OcSetupData.TurboPowerLimitLock != (UINT8) BiosSettingValue) {
             OcSetupData.TurboPowerLimitLock = (UINT8) BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_OC_ENABLE:
           //
           // OC Enable in the XTU Spec, but OC Lock in BIOS, so BiosSettingValue is inverted
           // BiosSettingValue = 0 means OC disabled, so OC Lock = 1
           // BiosSettingValue = 1 means OC enabled, so OC Lock = 0
           //
           if (OcSetupData.OverclockingLock != (UINT8) (~BiosSettingValue)) {
             OcSetupData.OverclockingLock = (UINT8) (~BiosSettingValue & BIT0);
             OcSetupData.OverclockingSupport = 1;
           }
           break;

        case BIOS_DEVICE_IGFX_FREQUENCY:
           if (OcSetupData.GtsMaxOcRatio != (UINT8) BiosSettingValue) {
             OcSetupData.GtsMaxOcRatio = (UINT8) BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_IGFX_UNSLICED_FREQUENCY:
           if (OcSetupData.GtusMaxOcRatio != (UINT8) BiosSettingValue) {
             OcSetupData.GtusMaxOcRatio = (UINT8) BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_IA_CORE_VOLTAGE_MODE:
           if (OcSetupData.CoreVoltageMode != (UINT8) BiosSettingValue) {
             OcSetupData.CoreVoltageMode = (UINT8) BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_CPU_VOLTAGE:
           //
           // Only 1 voltage target control ID. Voltage Mode will determine which value is applied to FIVR.
           // Need to keep both voltage targets in sync.
           //
           OcSetupData.CoreVoltageOverride = (UINT16)BiosSettingValue;
           OcSetupData.CoreExtraTurboVoltage = (UINT16)BiosSettingValue;
           break;

        case BIOS_DEVICE_IA_CORE_VOLTAGE_OFFSET:
           if (OcSetupData.CoreVoltageOffset != (UINT16) BiosSettingValue) {
             //
             // Need to determine if the requested number is negative
             // Voltage offset has a range of -1000 to 1000.
             // XTU passes in 0 to 2000 where 0 to 1000 maps to -1000 to 0
             // and 1001 to 2000 maps to 1 to 1000.
             //
             if (BiosSettingValue <= 1000) {
               //
               // This is a negative offset
               //
               OcSetupData.CoreVoltageOffset = 1000 - (UINT16)BiosSettingValue;
               OcSetupData.CoreVoltageOffsetPrefix = 1;
             }
             else if ((BiosSettingValue > 1000) && (BiosSettingValue <= 2000)) {
               //
               // This is a positive offset, and within range
               //
               OcSetupData.CoreVoltageOffset = (UINT16)BiosSettingValue - 1000;
               OcSetupData.CoreVoltageOffsetPrefix = 0;
             }
           }
           break;

        case BIOS_DEVICE_TURBO_BOOST_POWER_TIME_WINDOWS:
           if (OcSetupData.PowerLimit1Time != (UINT8) BiosSettingValue) {
             OcSetupData.PowerLimit1Time = (UINT8) BiosSettingValue;
           }
           break;

#if 0 // AMI_OVERRIDE_START
        //
        // ICC MAX
        //
        case BIOS_DEVICE_IA_CURRENT_LIMIT_MAX:
           if (OcSetupData.IccMax[1]!= (UINT8) BiosSettingValue) {
             OcSetupData.IccMax[1] = (UINT8) BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_SA_CURRENT_LIMIT_MAX:
           if (OcSetupData.IccMax[0]!= (UINT8) BiosSettingValue) {
             OcSetupData.IccMax[0] = (UINT8) BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_GTS_CURRENT_LIMIT_MAX:
           if (OcSetupData.IccMax[3]!= (UINT8) BiosSettingValue) {
             OcSetupData.IccMax[3] = (UINT8) BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_GTUS_CURRENT_LIMIT_MAX:
           if (OcSetupData.IccMax[2]!= (UINT8) BiosSettingValue) {
             OcSetupData.IccMax[2] = (UINT8) BiosSettingValue;
           }
           break;
#endif // AMI_OVERRIDE_END

        case BIOS_DEVICE_IA_AVX2_CORE_RATIO_OFFSET:
           if (OcSetupData.Avx2RatioOffset!= (UINT8) BiosSettingValue) {
             OcSetupData.Avx2RatioOffset = (UINT8) BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_XMP_PROFILE:
           if (OcSetupData.SpdProfileSelected != (UINT8) BiosSettingValue) {
             OcSetupData.SpdProfileSelected = (UINT8) BiosSettingValue;
//AMI_OVERRIDE_START - Fix EIP229470			 
			 if (BiosSettingValue == DEFAULT_SPD_PROFILE) {
			     OcSetupData.DdrOddRatioMode = DDR_ODD_RATIO_MODE_DISABLE;
			 }
//AMI_OVERRIDE_END - Fix EIP229470			 
           }
           break;

        case BIOS_DEVICE_MEMORY_CLOCK_FREQUENCY:
//AMI_OVERRIDE_START - Fix SetupData error DdrRefClk: 1 is 133 and 2 is 100, but OC 0 is 100, 1 is 133
#if 0
           if (OcSetupData.DdrRefClk != (UINT8) BiosSettingValue) {
             OcSetupData.DdrRefClk = (UINT8) BiosSettingValue;
           }
#else
           if(BiosSettingValue == 0) {
               OcSetupData.DdrRefClk = 2;
           } else {
               OcSetupData.DdrRefClk = 1;
           }
#endif
//AMI_OVERRIDE_END - Fix SetupData error		   
           break;

        case BIOS_DEVICE_RING_RATIO:
           if (OcSetupData.RingMaxOcRatio != (UINT8) BiosSettingValue) {
             OcSetupData.RingMaxOcRatio = (UINT8) BiosSettingValue;
             OcSetupData.OverclockingSupport = 1;
           }
           break;

        case BIOS_DEVICE_IGFX_VOLTAGE_MODE:
           if (OcSetupData.GtsVoltageMode != (UINT8) BiosSettingValue) {
             OcSetupData.GtsVoltageMode = (UINT8) BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_IGFX_VOLTAGE_OVERRIDE:
           //
           // Only 1 voltage target control ID. Voltage Mode will determine which value is applied to FIVR.
           // Need to keep both voltage targets in sync.
           //
           OcSetupData.GtsVoltageOverride = (UINT16)BiosSettingValue;
           OcSetupData.GtsExtraTurboVoltage = (UINT16)BiosSettingValue;
           break;

        case BIOS_DEVICE_IGFX_VOLTAGE_OFFSET:
           if (OcSetupData.GtsVoltageOffset != (UINT16) BiosSettingValue) {
             //
             // Need to determine if the requested number is negative
             // Voltage offset has a range of -1000 to 1000.
             // XTU passes in 0 to 2000 where 0 to 1000 maps to -1000 to 0
             // and 1001 to 2000 maps to 1 to 1000.
             //
             if (BiosSettingValue <= 1000) {
               //
               // This is a negative offset
               //
               OcSetupData.GtsVoltageOffset = 1000 - (UINT16)BiosSettingValue;
               OcSetupData.GtsVoltageOffsetPrefix = 1;
             }
             else if ((BiosSettingValue > 1000) && (BiosSettingValue <= 2000)) {
               //
               // This is a positive offset, and within range
               //
               OcSetupData.GtsVoltageOffset = (UINT16)BiosSettingValue - 1000;
               OcSetupData.GtsVoltageOffsetPrefix = 0;
             }
           }
           break;

        case BIOS_DEVICE_IGFX_UNSLICED_VOLTAGE_MODE:
           if (OcSetupData.GtsVoltageMode != (UINT8) BiosSettingValue) {
             OcSetupData.GtsVoltageMode = (UINT8) BiosSettingValue;
           }
           break;

        case BIOS_DEVICE_IGFX_UNSLICED_VOLTAGE_OVERRIDE:
           //
           // Only 1 voltage target control ID. Voltage Mode will determine which value is applied to FIVR.
           // Need to keep both voltage targets in sync.
           //
           OcSetupData.GtsVoltageOverride = (UINT16)BiosSettingValue;
           OcSetupData.GtsExtraTurboVoltage = (UINT16)BiosSettingValue;
           break;

        case BIOS_DEVICE_IGFX_UNSLICED_VOLTAGE_OFFSET:
           if (OcSetupData.GtsVoltageOffset != (UINT16) BiosSettingValue) {
             //
             // Need to determine if the requested number is negative
             // Voltage offset has a range of -1000 to 1000.
             // XTU passes in 0 to 2000 where 0 to 1000 maps to -1000 to 0
             // and 1001 to 2000 maps to 1 to 1000.
             //
             if (BiosSettingValue <= 1000) {
               //
               // This is a negative offset
               //
               OcSetupData.GtsVoltageOffset = 1000 - (UINT16)BiosSettingValue;
               OcSetupData.GtsVoltageOffsetPrefix = 1;
             }
             else if ((BiosSettingValue > 1000) && (BiosSettingValue <= 2000)) {
               //
               // This is a positive offset, and within range
               //
               OcSetupData.GtsVoltageOffset = (UINT16)BiosSettingValue - 1000;
               OcSetupData.GtsVoltageOffsetPrefix = 0;
             }
           }
           break;

        case BIOS_DEVICE_SA_VOLTAGE_OFFSET:
           if (OcSetupData.UncoreVoltageOffset != (UINT16) BiosSettingValue) {
             //
             // Need to determine if the requested number is negative
             // Voltage offset has a range of -1000 to 1000.
             // XTU passes in 0 to 2000 where 0 to 1000 maps to -1000 to 0
             // and 1001 to 2000 maps to 1 to 1000.
             //
             if (BiosSettingValue <= 1000) {
               //
               // This is a negative offset
               //
               OcSetupData.UncoreVoltageOffset = 1000 - (UINT16)BiosSettingValue;
               OcSetupData.UncoreVoltageOffsetPrefix = 1;
             }
             else if ((BiosSettingValue > 1000) && (BiosSettingValue <= 2000)) {
               //
               // This is a positive offset, and within range
               //
               OcSetupData.UncoreVoltageOffset = (UINT16)BiosSettingValue - 1000;
               OcSetupData.UncoreVoltageOffsetPrefix = 0;
             }
           }
           break;

        case BIOS_DEVICE_REAL_TIME_MEMORY_TIMING_ENABLE:
           if (OcSetupData.RealtimeMemoryTiming != (UINT8) BiosSettingValue) {
             OcSetupData.RealtimeMemoryTiming = (UINT8) BiosSettingValue;
           }
           break;
// AMI_OVERRIDE_START - Fix SetupData error and waring messages in BET tool.
        case BIOS_DEVICE_IA_CURRENT_LIMIT_MAX:
            if ((BiosSettingValue == 0xFFFFFFFE) && (OcSetupData.IccMax[1] == 0)) {
                break;
            }
            if (OcSetupData.IccMax[1] != (UINT16) BiosSettingValue) {
                if (BiosSettingValue == 0xFFFFFFFE) 
                    OcSetupData.IccMax[1] = 0;
                else
                    OcSetupData.IccMax[1] = (UINT16) BiosSettingValue;
                OcSetupData.VrConfigEnable[1] = 1;
            }
            break;
            
        case BIOS_DEVICE_GTUS_CURRENT_LIMIT_MAX:
            if ((BiosSettingValue == 0xFFFFFFFE) && (OcSetupData.IccMax[2] == 0)) {
                break;
            }
            if (OcSetupData.IccMax[2] != (UINT16) BiosSettingValue) {
                if (BiosSettingValue == 0xFFFFFFFE) 
                    OcSetupData.IccMax[2] = 0;
                else
                    OcSetupData.IccMax[2] = (UINT16) BiosSettingValue;
                OcSetupData.VrConfigEnable[2] = 1;
            }
            break;
            
        case BIOS_DEVICE_GTS_CURRENT_LIMIT_MAX:
            if ((BiosSettingValue == 0xFFFFFFFE) && (OcSetupData.IccMax[3] == 0)) {
                break;
            }
            if (OcSetupData.IccMax[3] != (UINT16) BiosSettingValue) {
                if (BiosSettingValue == 0xFFFFFFFE) 
                    OcSetupData.IccMax[3] = 0;
                else
                    OcSetupData.IccMax[3] = (UINT16) BiosSettingValue;
                OcSetupData.VrConfigEnable[3] = 1;
            }
            break;
// AMI_OVERRIDE_END - Fix SetupData error and waring messages in BET tool.
      }
// AMI_OVERRIDE_START - Fix SetupData error and waring messages in BET tool.
      if (PlatformFlavor == FlavorMobile) {
	    switch ( BiosSettingEntries[Index].BiosImplementationType ) {
		    case BIOS_DEVICE_SA_CURRENT_LIMIT_MAX:
			    if ((BiosSettingValue == 0xFFFFFFFE) && (OcSetupData.IccMax[0] == 0)) {
				    break;
			    }
			    if (OcSetupData.IccMax[0] != (UINT16) BiosSettingValue) {
				    if (BiosSettingValue == 0xFFFFFFFE)
					    OcSetupData.IccMax[0] = 0;
				    else
					    OcSetupData.IccMax[0] = (UINT16) BiosSettingValue;
				    OcSetupData.VrConfigEnable[0] = 1;
			    }
			    break;
	    }
      }
// AMI_OVERRIDE_END - Fix SetupData error and waring messages in BET tool.
    }

    OcSetupData.OcInterfaceRtUpdated = 1;
    Status = mSmmVariable->SmmSetVariable (
                                L"OcSetup",
                                &gOcSetupVariableGuid,
                                OcVariableAttributes,
                                OcVariableSize,
                                &OcSetupData
                                );

//AMI_OVERRIDE_START - Override for AMI SMM to access CPU register
#if 0    
    if(EFI_ERROR (Status)){
      (pCpuSaveState->x64)._RBX = (UINT32)0x0000FFFF;  // Internal BIOS Error
      (pCpuSaveState->x64)._RCX = (UINT32)Status;      // Error Code status
    }
#endif
      
  	if(EFI_ERROR (Status)){    	
    	ErrorCode = 0x0000FFFF;
    	Status = gSmmCpu->WriteSaveState ( gSmmCpu, \
    	                                   4, \
    	                                   EFI_SMM_SAVE_STATE_REGISTER_RBX, \
    	                                   Cpu, \
    	                                   &ErrorCode );
    	ErrorCode = (UINT32)Status;
    	Status = gSmmCpu->WriteSaveState ( gSmmCpu, \
    	                                   4, \
    	                                   EFI_SMM_SAVE_STATE_REGISTER_RCX, \
    	                                   Cpu, \
    	                                   &ErrorCode );
  	}
//AMI_OVERRIDE_END - Override for AMI SMM to access CPU register
    ASSERT_EFI_ERROR (Status);
  }

  if (ReadWrite == OVERCLOCK_SMI_WDT_CONFIRM_REQUEST_COMMAND) {
    OcSetupData.WDTSupportforNextOSBoot = 1;
    OcSetupData.OverclockingSupport     = ENABLE_SUPPRORT;

    switch (TimerValue) {
      case MAX_TIME_OUT:
        OcSetupData.TimeforNextOSBoot = 0xFFFF;
        break;

      case SUCCESSFUL_BOOT:
        OcSetupData.WDTSupportforNextOSBoot = 0;
        OcSetupData.TimeforNextOSBoot = 0;
        OcSetupData.PlatformUnstable = 0;
        break;

      default:
        OcSetupData.TimeforNextOSBoot = (UINT16)TimerValue;
        break;
    }
    OcSetupData.OcInterfaceRtUpdated = 1;
    Status = mSmmVariable->SmmSetVariable (
                                L"OcSetup",
                                &gOcSetupVariableGuid,
                                OcVariableAttributes,
                                OcVariableSize,
                                &OcSetupData
                                );
//AMI_OVERRIDE_START - Override for AMI SMM to access CPU register
#if 0    
    if (EFI_ERROR (Status)) {
      (pCpuSaveState->x64)._RBX = (UINT32)0x0000FFFF;  //Internal BIOS Error
      (pCpuSaveState->x64)._RCX = (UINT32)Status;  //Error Code status
    }
#endif	

    if(EFI_ERROR (Status)){    	
      ErrorCode = 0x0000FFFF;
     	Status = gSmmCpu->WriteSaveState ( gSmmCpu, \
      	                                   4, \
     	                                   EFI_SMM_SAVE_STATE_REGISTER_RBX, \
     	                                   Cpu, \
     	                                   &ErrorCode );
      ErrorCode = (UINT32)Status;
     	Status = gSmmCpu->WriteSaveState ( gSmmCpu, \
     	                                   4, \
     	                                   EFI_SMM_SAVE_STATE_REGISTER_RCX, \
     	                                   Cpu, \
     	                                   &ErrorCode );
    }
//AMI_OVERRIDE_END - Override for AMI SMM to access CPU register
    ASSERT_EFI_ERROR (Status);
  }

  return Status;
}

/**
  This function is called from SMM during SMM registration.

  @param[in] ImageHandle     Image handle of this driver.
  @param[in] SystemTable     Global system service table.

**/
EFI_STATUS
InSmmFunction (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_SMM_SW_DISPATCH2_PROTOCOL *pSwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT   SwContext;
  EFI_HANDLE                    Handle;
  EFI_STATUS                    Status;
  
//AMI_OVERRIDE_START - Override for AMI SMM to access CPU register
  Status = InitAmiSmmLib( ImageHandle, SystemTable );
  if (EFI_ERROR(Status)) return Status;
//AMI_OVERRIDE_END - Override for AMI SMM to access CPU register    

  Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID**)&pSwDispatch);
  ASSERT_EFI_ERROR (Status);
  
//AMI_OVERRIDE_START - Override for AMI SMM to access CPU register
  Status = pSmst->SmmLocateProtocol(&gEfiSmmCpuProtocolGuid, NULL, &gSmmCpu);
  if (EFI_ERROR(Status)) return Status;
//AMI_OVERRIDE_END - Override for AMI SMM to access CPU register

  //
  // Register SMIBiosSettings handler for SW SMI 0x72
  //
  SwContext.SwSmiInputValue = PcdGet8 (PcdOverClockingInterfaceSwSmi);
  Status = pSwDispatch->Register (pSwDispatch, SMIFlashSMIHandler, &SwContext, &Handle);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiSmmVariableProtocolGuid, NULL, (VOID**)&mSmmVariable);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Saves the XTU common buffer address created by Overclocking interface into SMRAM.
  This is done to prevent tampering of the smi common buffer address

  @param[in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]      Context         Points to an optional handler context which was specified when the
                                  handler was registered.

  @retval EFI_SUCCESS             The interrupt was handled successfully.
**/
EFI_STATUS
EFIAPI
SaveXtuSmiCommonBuffer (
  IN EFI_HANDLE                  DispatchHandle,
  IN EFI_SMM_SW_REGISTER_CONTEXT *DispatchContext
  )
{

  //
  // Save the XtuSmiBaseAddress into the SMRAM copy if address is unmodified.
  // This is to ensure the address isn't tampered during OS runtime.
  //
  if (mXtuSmiBaseAddress == 0) {
#if 0 // AMI_OVERRIDE_START - Fix Linux build error.
    (UINTN)mXtuSmiBaseAddress = (UINTN)mGlobalNvsAreaPtr->XtuSmiBaseAddress;
#else
    mXtuSmiBaseAddress = (UINTN)mGlobalNvsAreaPtr->XtuSmiBaseAddress;
#endif // AMI_OVERRIDE_END - Fix Linux build error.
    DEBUG ((DEBUG_INFO, "OC SMI: SaveXtuSmiCommonBuffer mXtuSmiBaseAddress = %X\n",mXtuSmiBaseAddress ));
  }

  return EFI_SUCCESS;
}

/**
  This is the entrypoint of the SMIBiosSettings driver

  @param[in] ImageHandle     Image handle of this driver.
  @param[in] SystemTable     Global system service table.

**/
EFI_STATUS
EFIAPI
OverClockSmiHandlerEntryPoint (
  IN  EFI_HANDLE       ImageHandle,
  IN  EFI_SYSTEM_TABLE *SystemTable
  )
{
  UINT32 Data32;
  EFI_SMM_SW_DISPATCH2_PROTOCOL  *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT    SwContext;
  EFI_HANDLE                     SwHandle;
  EFI_STATUS Status;
//AMI_OVERRIDE_START - Override for AMI SMM access
  UINTN                          VariableSize;
  UINT32                         SetupVariableAttributes;
  SETUP_DATA                     SetupData;
//AMI_OVERRIDE_END - Override for AMI SMM access
// AMI_OVERRIDE_START - Fix waring messages in BET tool.
  PLATFORM_INFO                  *PlatformInfo;
// AMI_OVERRIDE_END - Fix waring messages in BET tool.
  //
  // Read MSR for Active Core and Thread Count.
  //
  Data32 = (UINT32) AsmReadMsr64 (MSR_CORE_THREAD_COUNT);
  mNumberOfActiveCores  = (UINT16) ((Data32 >> 16) & 0xFFFF);
  DEBUG ((DEBUG_INFO, "OC SMI: OverclockSmi Entry Point.\n"));

  Status = gBS->LocateProtocol(&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &mGlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "OC SMI: GlobalNvsAreaProtocol->Area = %X\n", mGlobalNvsAreaProtocol->Area));
  mGlobalNvsAreaPtr = mGlobalNvsAreaProtocol->Area;

  //
  // Initialize to zero to ensure BIOS is the only entity updating this variable
  //
  mXtuSmiBaseAddress = 0;


  //
  // Register XTU Common Buffer SMI handler
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID**)&SwDispatch);
  ASSERT_EFI_ERROR (Status);
  SwContext.SwSmiInputValue = SW_SMI_XTU_COMMON_BUFFER;
  Status = SwDispatch->Register (SwDispatch, (EFI_SMM_HANDLER_ENTRY_POINT2) SaveXtuSmiCommonBuffer, &SwContext, &SwHandle);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "OverClockInterface: mGlobalNvsAreaPtr->XtuSmiBaseAddress = 0x%X\n", mGlobalNvsAreaPtr->XtuSmiBaseAddress));

//AMI_OVERRIDE_START - Variable size isn't defined in function OverClockSmiHandlerEntryPoint.
  VariableSize = sizeof (SETUP_DATA);
//AMI_OVERRIDE_END - Variable size isn't defined in function OverClockSmiHandlerEntryPoint.  

//AMI_OVERRIDE_START - Override for AMI SMM access
  InitAmiLib (ImageHandle, SystemTable);

  Status = pRS->GetVariable (
                      L"Setup",
                      &gSetupVariableGuid,
                      &SetupVariableAttributes,
                      &VariableSize,
                      &SetupData
                      );
  ASSERT_EFI_ERROR (Status);
  //DEBUG((EFI_D_INFO, "OverClockSmiHandler : SetupData Attributes = 0x%x\n", Attributes));

  if (SetupData.OverclockingSupport == 0) { 
    return EFI_SUCCESS;
  }
//AMI_OVERRIDE_END - Override for AMI SMM access
  
// AMI_OVERRIDE_START - Fix waring messages in BET tool.
  Status = gBS->LocateProtocol (&gPlatformInfoProtocolGuid, NULL, (VOID **) &PlatformInfo);
  ASSERT_EFI_ERROR (Status);
  PlatformFlavor = PlatformInfo->PlatformFlavor;
  if (PlatformFlavor == FlavorMobile) {
	  NO_OF_BIOS_SETTING_STATIC_ENTRIES = 38; // 40 - 9 + 5(IccMax) - 3(RING_VOLTAGE) - 1(RING_IccMax) + 1(AVX ratio offset) + 4 (IGFX_UNSLICED VOLTAGE_MODE/VOLTAGE_OVERRIDE/VOLTAGE_OFFSET/FREQUENCY) + 1 (BIOS_DEVICE_REAL_TIME_MEMORY_TIMING_ENABLE)
  }else{
	  NO_OF_BIOS_SETTING_STATIC_ENTRIES = 37; // 40 - 9 + 5(IccMax) - 3(RING_VOLTAGE) - 1(RING_IccMax) + 1(AVX ratio offset) - 1(SA_IccMax) + 4 (IGFX_UNSLICED VOLTAGE_MODE/VOLTAGE_OVERRIDE/VOLTAGE_OFFSET/FREQUENCY) + 1 (BIOS_DEVICE_REAL_TIME_MEMORY_TIMING_ENABLE)
  }
// AMI_OVERRIDE_END - Fix waring messages in BET tool.
  
  return InSmmFunction (ImageHandle, SystemTable);
}
