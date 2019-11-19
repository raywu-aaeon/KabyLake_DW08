//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiMemoryInfoConfig.c
    This file for update AmiMemoryInfo Configuration.

**/
#include <AmiDxeLib.h>

#include <Library/BaseLib.h>
#include <Library/HobLib.h>

#include <Protocol/AmiMemInfo.h>
#include <Protocol/SaPolicy.h>
#include <Protocol/MemInfo.h>

#include <MemInfoHob.h>
#include <SaRegs.h>


extern  EFI_GUID  gAmiMemoryInfoGuid;

// KabylakeSiliconPkg\SystemAgent\Library\Private\DxeSmbiosMemoryLib\SmbiosMemory.h
///
/// Maximum rank memory size supported by the memory controller: 8GB (in terms of KB) for DDR4 and 4 GB for other types
///
#define MAX_RANK_CAPACITY       (4 * 1024 * 1024)
#define MAX_RANK_CAPACITY_DDR4  (8 * 1024 * 1024)

#define MEGABYTE                10

AMI_MEMORY_INFO    AmiMemoryInfoConfig;

/**
    This function is used to print AmiMemInfo protocol

    @param AmiMemoryInfoConfig is AmiMemInfo protocol

    @retval void

**/
VOID
PrintAmiMemInfo (
  AMI_MEMORY_INFO           AmiMemoryInfoConfig
  )
{
  UINT8    node;
  UINT8    Ch;
  UINT8    Dimm;

  for (node = 0; node < NODE_NUM; node++ ) {

    DEBUG((DEBUG_INFO, "RamType: %d\n",         AmiMemoryInfoConfig.MemoryArray[node].RamType));
    DEBUG((DEBUG_INFO, "MaxCapacity: %d MiB\n", AmiMemoryInfoConfig.MemoryArray[node].MaxCapacity));
    DEBUG((DEBUG_INFO, "Speed: %d Mhz\n",       AmiMemoryInfoConfig.MemoryArray[node].Speed));
    DEBUG((DEBUG_INFO, "TotalMemory: %d MiB\n", AmiMemoryInfoConfig.MemoryArray[node].TotalMemory));

    for (Ch = 0; Ch < CH_NUM; Ch++) {
      for (Dimm = 0; Dimm < DIMM_NUM; Dimm++) {
        DEBUG((DEBUG_INFO, "--- DIMM Slot %0d --- \n", ((node << 2) + (Ch << 1) + Dimm) ));
        DEBUG((DEBUG_INFO, "Size: %d MiB\n",     AmiMemoryInfoConfig.MemoryArray[node].Slot[(node << 2) + (Ch << 1) + Dimm].Size));
        DEBUG((DEBUG_INFO, "Ecc: %d\n",          AmiMemoryInfoConfig.MemoryArray[node].Slot[(node << 2) + (Ch << 1) + Dimm].Ecc));
        DEBUG((DEBUG_INFO, "SpdAddr: 0x%X\n",    AmiMemoryInfoConfig.MemoryArray[node].Slot[(node << 2) + (Ch << 1) + Dimm].SpdAddr));
        DEBUG((DEBUG_INFO, "DoubleSide: 0x%X\n", AmiMemoryInfoConfig.MemoryArray[node].Slot[(node << 2) + (Ch << 1) + Dimm].DoubleSide));
      }
    }

  }

}

/**
    This function is used to sync "MrcInterface.h" & "AmiMemInfo.h" (Memory Type definition)

    @param MrcDdrType The memory definition which is written in "MrcInterface.h"

    @retval The memory definition which is written in AmiMemInfo.h.

**/
MEMORY_TYPE_INFO
ConvertMrcDdrType (
  UINT8   DdrType
  )
{
  MEMORY_TYPE_INFO    Result = UnknownType;

  if (DdrType == MRC_DDR_TYPE_DDR4) {
    Result = DDR4;
  } else if (DdrType == MRC_DDR_TYPE_DDR3) {
    Result = DDR3;
  } else if (DdrType == MRC_DDR_TYPE_LPDDR3) {
    Result = LPDDR3;
  } else {
    Result = UnknownType;
  }

  return Result;
}


/**
    This function is the entry point for this DXE. This function
    initializes the AmiMemoryInfo protocol.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.
**/
EFI_STATUS
EFIAPI
AmiMemoryInfoConfigEntryPoint (
    IN EFI_HANDLE                     ImageHandle,
    IN EFI_SYSTEM_TABLE               *SystemTable
    )
{
  MEMORY_INFO_DATA_HOB      *mMemInfo;
  SA_POLICY_PROTOCOL        *SaPolicy;
  MEMORY_DXE_CONFIG         *MemoryDxeConfig;
  EFI_HANDLE                Handle = 0;
  EFI_STATUS                Status = EFI_SUCCESS;
  UINT8                     node;
  UINT8                     Ch;
  UINT8                     Dimm;
  UINT8                     RankInDIMM;
  UINT8                     ChannelASlotMap;
  UINT8                     ChannelBSlotMap;
  UINT8                     BitIndex;
  UINT16                    MaxSockets;
  UINT8                     ChannelASlotNum;
  UINT8                     ChannelBSlotNum;
  CONST UINT64              BaseOfEvenNum = 2;
  UINT64                    Remainder;
  EFI_HOB_GUID_TYPE         *GuidHob;

  InitAmiLib(ImageHandle, SystemTable);
  
  DEBUG((DEBUG_INFO, "AmiMemoryInfoConfig - START\n"));

  Handle = ImageHandle;
  
  //
  // Get the HOB list and install MemInfo protocol
  //
  GuidHob = GetFirstGuidHob (&gSiMemoryInfoDataGuid);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  mMemInfo = (MEMORY_INFO_DATA_HOB *) GET_GUID_HOB_DATA (GuidHob);

  ///
  /// Get the SA policy.
  ///
  Status = pBS->LocateProtocol (
                  &gSaPolicyProtocolGuid,
                  NULL,
                  (VOID **) &SaPolicy
                  );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "LocateProtocol failed: %r\n", Status));
    return Status;
  }

  ///
  /// Get the MemoryDxeConfig.
  ///
  Status = GetConfigBlock (
             (VOID *) SaPolicy, 
             &gMemoryDxeConfigGuid, 
             (VOID *)&MemoryDxeConfig
             );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "GetConfigBlock failed: %r\n", Status));
    return Status;
  }
  
  for (node = 0; node < NODE_NUM; node++) {
    //
    // To sync "MrcInterface.h" & "AmiMemInfo.h" (Memory Type definition)
    //
    AmiMemoryInfoConfig.MemoryArray[node].RamType = ConvertMrcDdrType ((UINT8) mMemInfo->MemoryType);
    
    //
    // Get the Memory DIMM info from policy protocols
    //
    ChannelASlotMap = MemoryDxeConfig->ChannelASlotMap;
    ChannelBSlotMap = MemoryDxeConfig->ChannelBSlotMap;
    ChannelASlotNum = 0;
    ChannelBSlotNum = 0;
    for (BitIndex = 0; BitIndex < 8; BitIndex++) {
      if ((ChannelASlotMap >> BitIndex) & BIT0) {
        ChannelASlotNum++;
      }

      if ((ChannelBSlotMap >> BitIndex) & BIT0) {
        ChannelBSlotNum++;
      }
    }
    MaxSockets = ChannelASlotNum + ChannelBSlotNum;

    //
    // Max size support (Unit = MB)
    //
    if (AmiMemoryInfoConfig.MemoryArray[node].RamType == DDR4) {
      AmiMemoryInfoConfig.MemoryArray[node].MaxCapacity = (MAX_RANK_CAPACITY_DDR4 * SA_MC_MAX_SIDES * MaxSockets) >> MEGABYTE; // Unit: MB.
    } else {
      AmiMemoryInfoConfig.MemoryArray[node].MaxCapacity = (MAX_RANK_CAPACITY * SA_MC_MAX_SIDES * MaxSockets) >> MEGABYTE; // Unit: MB.
    }


    //
    // Follow RC code to assign the value (Unit = Mhz)
    //
    AmiMemoryInfoConfig.MemoryArray[node].Speed       = (UINT16) mMemInfo->ConfiguredMemoryClockSpeed;   // Unit: Mhz
    //
    // Initialize with 0 (Unit = MiB)
    //
    AmiMemoryInfoConfig.MemoryArray[node].TotalMemory = (UINT32) mMemInfo->TotalPhysicalMemorySize;
    
    //
    // Porting AmiMemoryInfoConfig.
    //
    for (Ch = 0; Ch < CH_NUM; Ch++) {
      for (Dimm = 0; Dimm < DIMM_NUM; Dimm++) {

        AmiMemoryInfoConfig.MemoryArray[node].Slot[(node << 2) + (Ch << 1) + Dimm].Size =
        (UINT16) mMemInfo->Controller[0].ChannelInfo[Ch].DimmInfo[Dimm].DimmCapacity; // Unit: MB.

        AmiMemoryInfoConfig.MemoryArray[node].Slot[(node << 2) + (Ch << 1) + Dimm].Ecc =
        (BOOLEAN) mMemInfo->EccSupport;

        AmiMemoryInfoConfig.MemoryArray[node].Slot[(node << 2) + (Ch << 1) + Dimm].SpdAddr =
        (UINT8) MemoryDxeConfig->SpdAddressTable[(node << 2) + (Ch << 1) + Dimm];

        RankInDIMM = mMemInfo->Controller[0].ChannelInfo[Ch].DimmInfo[Dimm].RankInDimm;

        DEBUG((DEBUG_INFO, "RankInDIMM : 0x%X\n", RankInDIMM));

        DivU64x64Remainder (RankInDIMM, BaseOfEvenNum, &Remainder);
        if ( (Remainder == 0)    // RankInDIMM is even.
          && (RankInDIMM != 0)) {
          AmiMemoryInfoConfig.MemoryArray[node].Slot[(node << 2) + (Ch << 1) + Dimm].DoubleSide = TRUE;
        } else {                 // RankInDIMM is odd or DIMM is not present.
          AmiMemoryInfoConfig.MemoryArray[node].Slot[(node << 2) + (Ch << 1) + Dimm].DoubleSide = FALSE;
        }

      }
    }
  }

  //
  // Print AmiMemInfo
  //
  PrintAmiMemInfo(AmiMemoryInfoConfig);

  Status = pBS->InstallProtocolInterface (
                  &Handle,
                  &gAmiMemoryInfoGuid,
                  EFI_NATIVE_INTERFACE,
                  &AmiMemoryInfoConfig
                  );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "InstallMultipleProtocolInterfaces failed: %r\n", Status));
    return Status;
  }

  DEBUG((DEBUG_INFO, "AmiMemoryInfo protocol installed!!\n"));
  DEBUG((DEBUG_INFO, "AmiMemoryInfoConfig - END\n"));

  return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
