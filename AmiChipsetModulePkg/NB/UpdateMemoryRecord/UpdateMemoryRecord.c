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

/** @file UpdateMemoryRecord.c
    This file for update SMBIOS Type 16 ~19.

**/

#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Library/DebugLib.h>
#if (defined(SMBIOS_SUPPORT) && (SMBIOS_SUPPORT == 1))
#include <Protocol/AmiSmbios.h>
#endif
#include "UpdateMemoryRecord.h"
#include <Protocol/DataHub.h>
#include <Protocol/SmbiosUpdateDataProtocol.h>
#include <Guid/DataHubRecords.h>
#include <SaRegs.h>
#include <Protocol/MemInfo.h>
#include <Protocol/SmbusHc.h>
#include <Protocol/SaPolicy.h>
#include <Library/MemoryAllocationLib.h>

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
EFI_GUID  gNbSmbiosType17ExtraInfoDataGuid = NB_SMBIOS_TYPE17_EXTRA_INFO_DATA_GUID;

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
AMI_SMBIOS_PROTOCOL             *gAmiSmbiosProtocol;
EFI_SMBIOS_UPDATE_DATA_PROTOCOL *gSmbiosUpdateDataProtocol;
MEM_INFO_PROTOCOL               *gMemInfoProtocol = NULL;
EFI_SMBUS_HC_PROTOCOL           *gSmbusProtocol = NULL;
MEMORY_DXE_CONFIG               *mMemoryDxeConfig= NULL;

// AMI_OVERRIDE_NB0001_START >>>
UINT16   T17_Handle_Tmp = 0xFFFF;
BOOLEAN  Lock_T17_Handle_Tmp = FALSE;
// AMI_OVERRIDE_NB0001_END <<<

//----------------------------------------------------------------------------
VOID
InstallSmbiosType20 (
    VOID
)
{
    SMBIOS_MEM_DEV_MAP_ADDR_INFO    *TypeBuffer;
    UINT16                          bsize;
    UINT8                           ChannelASlotMap;
    UINT8                           ChannelBSlotMap;
    UINT8                           BitIndex;
    UINT16                          MaxSockets;
    UINT8                           ChannelASlotNum;
    UINT8                           ChannelBSlotNum;
    UINT8                           Dimm;
    BOOLEAN                         SlotPresent;
    BOOLEAN                         Populated;
    EFI_PHYSICAL_ADDRESS            BaseAddress = 0;
    EFI_PHYSICAL_ADDRESS            RowArrayBaseAddress[SA_MC_MAX_SOCKETS] = {0};
    UINT64                          RowArrayLength[SA_MC_MAX_SOCKETS] = {0};

    DEBUG((DEBUG_INFO, "BuildType20 \n"));

    ChannelASlotMap = mMemoryDxeConfig->ChannelASlotMap;
    ChannelBSlotMap = mMemoryDxeConfig->ChannelBSlotMap;
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
    MaxSockets  = ChannelASlotNum + ChannelBSlotNum;

    for (Dimm = 0; Dimm < SA_MC_MAX_SLOTS; Dimm++) {

        // Channel 0, If the DIMM is not present, don't calculate Physical address.
        if (gMemInfoProtocol->MemInfoData.DimmStatus[Dimm] == DIMM_PRESENT) {
            RowArrayBaseAddress[Dimm]  = BaseAddress;
            RowArrayLength[Dimm]    = LShiftU64 (gMemInfoProtocol->MemInfoData.dimmSize[Dimm], 20);
            BaseAddress += RowArrayLength[Dimm];
        }

        // Channel 1, If the DIMM is not present, don't calculate Physical address.
        if (gMemInfoProtocol->MemInfoData.DimmStatus[Dimm + SA_MC_MAX_SLOTS] == DIMM_PRESENT) {
            RowArrayBaseAddress[Dimm + SA_MC_MAX_SLOTS]  = BaseAddress;
            RowArrayLength[Dimm + SA_MC_MAX_SLOTS]    = LShiftU64 (gMemInfoProtocol->MemInfoData.dimmSize[Dimm + SA_MC_MAX_SLOTS], 20);
            BaseAddress += RowArrayLength[Dimm + SA_MC_MAX_SLOTS];
        }

    }

    for (Dimm = 0; Dimm < SA_MC_MAX_SOCKETS; Dimm++) {
        // Use channel slot map to check whether the Socket is supported in this SKU, some SKU only has 2 Sockets totally
        SlotPresent = FALSE;
        if (Dimm < 2) {
            if (ChannelASlotMap & (1 << Dimm)) {
            SlotPresent = TRUE;
            }
        } else {
            if (ChannelBSlotMap & (1 << (Dimm - 2))) {
            SlotPresent = TRUE;
            }
        }

        // Don't create Type 17 and Type 20 items for non-existing socket
        if (!SlotPresent) {
            continue;
        }
        Populated = (gMemInfoProtocol->MemInfoData.DimmStatus[Dimm] == DIMM_PRESENT);

        if ((Populated) && (RowArrayLength[Dimm] != 0)) {
            // Allocate temporary buffer
            TypeBuffer = AllocateZeroPool(sizeof(SMBIOS_MEM_DEV_MAP_ADDR_INFO) + (SIZE_1KB/4) );

            // Build header of structure
            TypeBuffer->StructureType.Type       = 20;  // Type 20
            TypeBuffer->StructureType.Length     = sizeof(SMBIOS_MEM_DEV_MAP_ADDR_INFO);

            // Build Content of structure
            TypeBuffer->StartingAddress          = (UINT32)(RowArrayBaseAddress[Dimm] / 1024);
            TypeBuffer->EndingAddress            = (UINT32)((RowArrayBaseAddress[Dimm] + RowArrayLength[Dimm] - 1) / 1024);

            TypeBuffer->MemoryDeviceHandle       = SMBIOS_UNKNOWN;
            TypeBuffer->MemoryArrayMapAddrHandle = SMBIOS_FindStructureHandle(19, 1);
            TypeBuffer->PartitionRowPosition     = SMBIOS_PARTITION_ROW_POSITION;

            // Set Interleave Data Depth and Position.
            if (((Dimm >= SA_MC_MAX_SLOTS) && (gMemInfoProtocol->MemInfoData.DimmStatus[Dimm - SA_MC_MAX_SLOTS] < DIMM_NOT_PRESENT )) ||
                ((Dimm < SA_MC_MAX_SLOTS) && (gMemInfoProtocol->MemInfoData.DimmStatus[Dimm + SA_MC_MAX_SLOTS] < DIMM_NOT_PRESENT))
                ) {
                // If DIMMs on both channels are populated then Interleaved Data Depth is 2 and Interleave Position is 1 or 2
                TypeBuffer->InterleaveDataDepth = (UINT8) (MaxSockets >> 1);
                TypeBuffer->InterleavePosition = (UINT8) (1 << (Dimm >= (MaxSockets >> 1)));
            } else {
                // AMI_OVERRIDE_NB0001_START >>>
                // If the device is not part of an interleave, the field contains 0.
                TypeBuffer->InterleaveDataDepth  = 0;
                TypeBuffer->InterleavePosition   = 0;
                // AMI_OVERRIDE_NB0001_END <<<
            }

            // Write structure To SMBIOS Buffer
            bsize = SMBIOS_GetStructureTotalSize((UINT8 *)TypeBuffer);
            gAmiSmbiosProtocol->SmbiosAddStructure((UINT8 *)TypeBuffer, bsize);

            // AMI_OVERRIDE_NB0001_START >>>
            Lock_T17_Handle_Tmp = FALSE;
            // AMI_OVERRIDE_NB0001_END <<<
            
            // Fix handle link
            SMBIOS_FixHandleLink(gAmiSmbiosProtocol->SmbiosGetTableEntryPoint());

            // Free temporary buffer
            pBS->FreePool(TypeBuffer);
        }
    }
}


#ifdef NB_SMBIOS_TYPE_17_CUSTOMIZATION_FLAG
UINT16
MemoryFrequencyLookUpTableDDR4 (
    IN UINT8   tCKmin
){
    //
    // Please refer JEDEC Standard No.21-C, SPD for DDR4
    // Byte 12: SDRAM Minimum Cycle Time (tCKmin)
    //
    if ( tCKmin == 0xA ) {
        return f1600;
    } else if ( tCKmin == 0x9 ) {
        return f1867;
    } else if ( tCKmin == 0x8 ) {
        return f2133;
    } else if ( tCKmin == 0x7 ) {
        return f2400;
    } else if ( tCKmin == 0x6 ) {
        return f2667;
    } else if ( tCKmin == 0x5 ) {
        return f3200;
    } else {
        return fNoInit;
    }
}

UINT16
MemoryFrequencyLookUpTableDDR3 (
    IN UINT8   tCKmin
){
    //
    // Please refer JEDEC Standard No.21-C, SPD for DDR4
    // Byte 18: SDRAM Minimum Cycle Time (tCKmin)
    //
    if ( tCKmin == 0x14 ) {
        return f800;
    } else if ( tCKmin == 0xF ) {
        return f1067;
    } else if ( tCKmin == 0xC ) {
        return f1333;
    } else if ( tCKmin == 0xA ) {
        return f1600;
    } else if ( tCKmin == 0x9 ) {
        return f1867;
    } else if ( tCKmin == 0x8 ) {
        return f2133;
    } else {
        return fNoInit;
    }
}

VOID
MinimumMaximumVoltageLookUpTableDDR3 (
    IN  UINT8 VDD,
    OUT SMBIOS_MEMORY_DEVICE_INFO *T17_p
){
    //
    // Please refer JEDEC Standard No.21-C, SPD for DDR3
    // Byte 6: Module Nominal Voltage, VDD
    //
    if (VDD & BIT2) { // 1.25v
        T17_p->MinimumVoltage = 1250;
    } else if (VDD & BIT1) { // 1.35v
        T17_p->MinimumVoltage = 1350;
    } else if(!(VDD & BIT0)) { // 1.5v bit0 = 0
        T17_p->MinimumVoltage = 1500;
    } else {
        T17_p->MinimumVoltage = 0;
    }

    if (!(VDD & BIT0)) { // 1.5v bit0 = 0
        T17_p->MaximumVoltage = 1500;
    } else if (VDD & BIT1) { // 1.35v
        T17_p->MaximumVoltage = 1350;
    } else if(VDD & BIT2) { // 1.25v
        T17_p->MaximumVoltage = 1250;
    } else {
        T17_p->MaximumVoltage = 0;
    }
}

VOID
MinimumMaximumVoltageLookUpTableDDR4 (
    IN  UINT8 VDD,
    OUT SMBIOS_MEMORY_DEVICE_INFO *T17_p
){
    //
    // Please refer JEDEC Standard No.21-C, SPD for DDR4
    // Byte 11: Module Nominal Voltage, VDD
    //
    if (VDD & BIT0) { // 1.2v
        T17_p->MinimumVoltage = 1200;
        T17_p->MaximumVoltage = 1200;
    } else {
        //
        // DRAM VDD TBD
        //
        T17_p->MinimumVoltage = 0;
        T17_p->MaximumVoltage = 0;
    }
}

/**
    This function is runtime update smbios type 17 for customized table
    Detail:
     1. Offset 15h - Speed
     2. Offset 22h - Minimum Voltage
     3. Offset 23h - Maximum Voltage

    @param No

    @retval No
**/
VOID
UpdateSmbiosType17Customization ( 
  VOID
){
    EFI_STATUS                  Status;
    UINT8                       Dimm, DdrType, Index;
    UINT16                      handle;
    SMBIOS_MEMORY_DEVICE_INFO   *T17_p;
    UINT8                       ChannelASlotMap;
    UINT8                       ChannelBSlotMap;
    UINT8                       BitIndex;
    UINT8                       ChannelASlotNum = 0;
    UINT8                       ChannelBSlotNum = 0;
    UINT8                       tCKmin = 0;
    UINT8                       VDD = 0;
    UINT16                      DeviceFreqMax = 0;
    NB_SMBIOS_TYPE_17_DATA_HOB  *T17_DataHob;
    VOID                        *FirstHob;

    if (gMemInfoProtocol == NULL || mMemoryDxeConfig == NULL) return;

    DEBUG((DEBUG_INFO, "UpdateSmbiosType17Customization() --- Start \n"));
    
    DdrType = gMemInfoProtocol->MemInfoData.DdrType;
    ChannelASlotMap = mMemoryDxeConfig->ChannelASlotMap;
    ChannelBSlotMap = mMemoryDxeConfig->ChannelBSlotMap;
    
    //
    // Get the HOB list (NB_SMBIOS_TYPE_17_DATA_HOB)
    //
    FirstHob = GetEfiConfigurationTable(pST, &gEfiHobListGuid);
    if (!FirstHob) ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);

    T17_DataHob = (NB_SMBIOS_TYPE_17_DATA_HOB *) FirstHob;

    while (!EFI_ERROR (Status = FindNextHobByType (EFI_HOB_TYPE_GUID_EXTENSION, (VOID **)&T17_DataHob))) {
      if (guidcmp (&T17_DataHob->EfiHobGuidType.Name, &gNbSmbiosType17ExtraInfoDataGuid) == 0) {
        break;
      }
    }

    for (BitIndex = 0; BitIndex < 8; BitIndex++) {
        if ((ChannelASlotMap >> BitIndex) & BIT0) {
        ChannelASlotNum++;
        }

        if ((ChannelBSlotMap >> BitIndex) & BIT0) {
        ChannelBSlotNum++;
        }
    }
    
    for (Dimm = 0; Dimm < SA_MC_MAX_SOCKETS; Dimm++) {

        if (gMemInfoProtocol->MemInfoData.DimmStatus[Dimm] != DIMM_PRESENT) continue;

        Index = Dimm + 1;
        if (Dimm < 2 && (ChannelASlotNum == 1))
            Index = 1;
        if (Dimm >= 2 && (ChannelBSlotNum == 1))
            Index = ChannelASlotNum + 1;

        //
        // Update Speed
        //
        tCKmin = T17_DataHob->NbSpdData[Dimm].tCKmin;

        //
        // tCKmin convert to Memory Frequency  by lookup table
        //
        if (DdrType == MRC_DDR_TYPE_DDR4) {
            DeviceFreqMax = MemoryFrequencyLookUpTableDDR4(tCKmin);
        } else {
            DeviceFreqMax = MemoryFrequencyLookUpTableDDR3(tCKmin);
        }

        DEBUG((DEBUG_INFO, "tCKmin: %d\n", tCKmin));
        DEBUG((DEBUG_INFO, "DeviceFreqMax: %d\n", DeviceFreqMax));

        //
        // Update Minimum/Maximum memory voltage
        //
        VDD = T17_DataHob->NbSpdData[Dimm].VDD;

        handle = gSmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(17, Index);
        if (handle != SMBIOS_NOT_FOUND) {

            T17_p = (SMBIOS_MEMORY_DEVICE_INFO *)gSmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(handle);

            //
            // Runtime update Speed, Configured Memory Clock Speed
            //
            if (DeviceFreqMax) {
                T17_p->Speed = DeviceFreqMax;
            } else {
                T17_p->Speed = gMemInfoProtocol->MemInfoData.ddrFreq;
            }
            T17_p->ConfMemClkSpeed = gMemInfoProtocol->MemInfoData.ddrFreq;
            handle = SMBIOS_NOT_FOUND;

            //
            // Runtime update Minimum/Maximum memory voltage
            //
            switch (DdrType) {
            case MRC_DDR_TYPE_DDR3:
                MinimumMaximumVoltageLookUpTableDDR3 (VDD, T17_p);
                break;
                
            case MRC_DDR_TYPE_LPDDR3:   // [ EIP433144 ]
                T17_p->MinimumVoltage = 1200;
                T17_p->MaximumVoltage = 1200;
                break;
                
            case MRC_DDR_TYPE_DDR4:
                MinimumMaximumVoltageLookUpTableDDR4 (VDD, T17_p);
                break;

            default:
                T17_p->MinimumVoltage = 0;
                T17_p->MaximumVoltage = 0;
                break;
            }
        }
    }

    DEBUG((DEBUG_INFO, "UpdateSmbiosType17Customization() --- End \n"));
}
#endif

/**
    Updates Memory related structures (Type 16-20) in
    input Buffer with dynamically detected Record for Intel Tiano
    SmBiosMemory Driver.

    @param Buffer 

    @retval EFI_STATUS EFI_SUCCESS

**/
EFI_STATUS
DynamicUpdateMemoryRecord(
    IN  SMBIOS_TABLE_ENTRY_POINT    *Buffer
)
{
    EFI_STATUS                          Status; 
    SA_POLICY_PROTOCOL                  *SaPolicy;
#if defined (TYPE16_STRUCTURE) && (TYPE16_STRUCTURE == 1)
    UINT8                           ChannelASlotMap;
    UINT8                           ChannelBSlotMap;
    UINT8                           BitIndex;
    UINT16                          MaxSockets;
    UINT8                           ChannelASlotNum;
    UINT8                           ChannelBSlotNum;
    UINT8                           Index;
#endif


    Status = pBS->LocateProtocol (&gMemInfoProtocolGuid, NULL, (VOID **) &gMemInfoProtocol);
    if (EFI_ERROR (Status))
    {
        ASSERT_EFI_ERROR (Status);
    }

    Status = pBS->LocateProtocol (&gEfiSmbusHcProtocolGuid, NULL, (VOID **) &gSmbusProtocol);
    if (EFI_ERROR (Status))
    {
        ASSERT_EFI_ERROR (Status);
    }


    ///
    /// Get the platform setup policy.
    ///
    Status = pBS->LocateProtocol (&gSaPolicyProtocolGuid, NULL, (VOID **)&SaPolicy);
    if (EFI_ERROR (Status))
    {
        ASSERT_EFI_ERROR (Status);
    }
    
    Status = GetConfigBlock ((VOID *)SaPolicy, &gMemoryDxeConfigGuid, (VOID *)&mMemoryDxeConfig);
    if (EFI_ERROR (Status))
    {
        ASSERT_EFI_ERROR (Status);
    }


#if defined (TYPE16_STRUCTURE) && (TYPE16_STRUCTURE == 1)
    ChannelASlotMap = mMemoryDxeConfig->ChannelASlotMap;
    ChannelBSlotMap = mMemoryDxeConfig->ChannelBSlotMap;
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
    MaxSockets  = ChannelASlotNum + ChannelBSlotNum;
   
    // Clean structures which need rebuild
    SMBIOS_DeleteStructureByType(16, 1);
    for (Index = 0; Index < MaxSockets; Index++) {
       SMBIOS_DeleteStructureByType(17, 1);
#if MEMORY_ERROR_INFO
       SMBIOS_DeleteStructureByType(18, 1);
#endif
    }
    SMBIOS_DeleteStructureByType(19, 1);
    SMBIOS_DeleteStructureByType(20, SMBIOS_FOR_ALL);
#endif

    //
    // Get all available data records from data hub
    //
#ifdef NB_SMBIOS_TYPE_17_CUSTOMIZATION_FLAG
    UpdateSmbiosType17Customization();
#endif

    InstallSmbiosType20();
    return EFI_SUCCESS;
}

/**
    Update memory record DXE driver for Intel Tiano SmBiosMemory Driver.

    @param ImageHandle Image handle.
    @param SystemTable Pointer to the system table.

    @retval EFI_STATUS EFI_SUCCESS.

**/
EFI_STATUS EFIAPI
UpdateMemoryRecordDriverEntryPoint (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS                  Status;
    SMBIOS_TABLE_ENTRY_POINT    *SmbiosTableEntryPoint;

    InitAmiLib(ImageHandle, SystemTable);

    Status = pBS->LocateProtocol(&gAmiSmbiosProtocolGuid, NULL, (VOID **)&gAmiSmbiosProtocol);
    if (EFI_ERROR (Status))
    {
        ASSERT_EFI_ERROR (Status);
    }

    Status = pBS->LocateProtocol(&gSmbiosUpdateDataProtocolGuid, NULL, (VOID **)&gSmbiosUpdateDataProtocol);
    if (EFI_ERROR (Status))
    {
        ASSERT_EFI_ERROR (Status);
    }


    SmbiosTableEntryPoint = gAmiSmbiosProtocol->SmbiosGetTableEntryPoint();

    Status = DynamicUpdateMemoryRecord(SmbiosTableEntryPoint);
    if (EFI_ERROR (Status))
    {
        ASSERT_EFI_ERROR (Status);
    }


    return EFI_SUCCESS;
}

//-----------------------------------------------------------------------------
// SMBIOS Dynamic Maintain Functions
//-----------------------------------------------------------------------------

/**
    SMBIOSUpdateData protocol - Searches handle of Smbios Table by Type

    @param Type 
    @param Index 

    @retval UINT16 Handle or -1(if not found)

**/
UINT16
SMBIOS_FindStructureHandle(
    IN UINT8	Type,
    IN UINT8	Index
)
{
    return gSmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(Type, Index);
}

/**
    SMBIOS Delete Structure Type

    @param SmbiosBuffer 
    @param Type 
    @param Index 

    @retval UINTN count

**/
UINTN
SMBIOS_DeleteStructureByType(
	IN UINT8	Type,
    IN UINT8	Index
)
{
    UINT8	i;
    UINT16	handle;
    UINTN	count;

    count = 0;
    if (Index != SMBIOS_FOR_ALL) {   		// for Single
        handle = gSmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(Type, Index);
        if (handle != SMBIOS_NOT_FOUND) {
            DEBUG((DEBUG_INFO, "SMBIOS_DeleteStructureByType - Type = %d\n", Type));
            gSmbiosUpdateDataProtocol->SMBIOS_DeleteStructure(handle);
        }
    }
    else { 									// -1 for ALL
		for (i = 0; i < MAX_HANDLES; i++) {
			handle = gSmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(Type, 1);
			if (handle != SMBIOS_NOT_FOUND)	{
	            DEBUG((DEBUG_INFO, "SMBIOS_DeleteStructureByType - Type = %d, Handle = %x\n", Type, handle));
				gSmbiosUpdateDataProtocol->SMBIOS_DeleteStructure(handle);
				count++;
			}
		}
    }

    return count;
}

/**
    SMBIOS get structure total size

    @param BufferStart 

    @retval UINT16 Total Size.

**/
UINT16
SMBIOS_GetStructureTotalSize(
    IN UINT8	*BufferStart
)
{
    UINT8 *BufferEnd;

    BufferEnd = BufferStart;
    BufferEnd += ((SMBIOS_STRUCTURE_HEADER*)BufferStart)->Length;
    while (*(UINT16*)BufferEnd != 0)
    {
        BufferEnd++;
    }

    return (UINT16)(BufferEnd + 2 - BufferStart);   // +2 for double zero terminator
}

/**
    SMBIOS fix handle link

    @param SmbiosBuffer 


    @retval VOID

**/
VOID
SMBIOS_FixHandleLink(
    IN  VOID *SmbiosBuffer
)
{
    UINT16                              handle;
    SMBIOS_PHYSICAL_MEM_ARRAY_INFO      *T16_p;
    SMBIOS_MEMORY_DEVICE_INFO           *T17_p;
    SMBIOS_MEM_ARRAY_MAP_ADDR_INFO      *T19_p;
    SMBIOS_MEM_DEV_MAP_ADDR_INFO        *T20_p;
    UINT16                              T19_Handle;
    // AMI_OVERRIDE_NB0001_START >>> 
    UINT32                              T17_Size_KB;
    UINT32                              T20_Size_KB;
    // AMI_OVERRIDE_NB0001_END <<<
    UINTN                               Q_flag;
    UINT8                               i, j;
#if MEMORY_ERROR_INFO
    SMBIOS_MEMORY_ERROR_INFO            *T18_p = NULL;
#endif

    T16_p               = NULL;
    T17_p               = NULL;
    T19_p               = NULL;
    T20_p               = NULL;
    T19_Handle          = SMBIOS_UNKNOWN;
    Q_flag              = 0;
    T17_Size_KB         = 0;

    // Looking for Type 19 to set MemoryArrayHandle
    handle = gSmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(19, 1);
    if (handle != SMBIOS_NOT_FOUND) {
        T19_p = (SMBIOS_MEM_ARRAY_MAP_ADDR_INFO *)gSmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(handle);
        T19_Handle = T19_p->StructureType.Handle;
        if (T19_p->MemoryArrayHandle == SMBIOS_UNKNOWN) {
            // Looking for Type 16
            handle = gSmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(16, 1);
            if (handle != SMBIOS_NOT_FOUND) {
                T16_p = (SMBIOS_PHYSICAL_MEM_ARRAY_INFO *)gSmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(handle);
                T19_p->MemoryArrayHandle = T16_p->StructureType.Handle;
            }
        }
    }

#if MEMORY_ERROR_INFO
    // Looking for Type 16
    handle = gSmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(16, 1);
    if (handle != SMBIOS_NOT_FOUND) {
        T16_p = (SMBIOS_PHYSICAL_MEM_ARRAY_INFO *)gSmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(handle);
        if (T16_p->MemErrInfoHandle == SMBIOS_UNKNOWN) {
             handle = gSmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(18, 1);
             if (handle != SMBIOS_NOT_FOUND) {
                 T18_p = (SMBIOS_MEMORY_ERROR_INFO *)gSmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(handle);
                 T16_p->MemErrInfoHandle = T18_p->StructureType.Handle;
             }
        }
    }

    handle = gSmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(17, 1);
    if (handle != SMBIOS_NOT_FOUND) {
        T17_p = (SMBIOS_MEMORY_DEVICE_INFO *)gSmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(handle);
        if (T17_p->MemErrorInfoHandle == SMBIOS_UNKNOWN) {
            handle = gSmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(18, 2);
            if (handle != SMBIOS_NOT_FOUND) {
                T18_p = (SMBIOS_MEMORY_ERROR_INFO *)gSmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(handle);
                T17_p->MemErrorInfoHandle = T18_p->StructureType.Handle;
            }
        }
    }

    handle = gSmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(17, 3);
    if (handle != SMBIOS_NOT_FOUND) {
        T17_p = (SMBIOS_MEMORY_DEVICE_INFO *)gSmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(handle);
        if (T17_p->MemErrorInfoHandle == SMBIOS_UNKNOWN) {
            handle = gSmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(18, 3);
            if (handle != SMBIOS_NOT_FOUND) {
                T18_p = (SMBIOS_MEMORY_ERROR_INFO *)gSmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(handle);
                T17_p->MemErrorInfoHandle = T18_p->StructureType.Handle;
            }
        }
    }
#endif

    Q_flag = 0;
    // Looking for Type 20 to set MemoryArrayMapAddrHandle and MemoryDeviceHandle
    for(i = 1; i < MAX_HANDLES; i++) {
        T20_Size_KB = 0;

        handle = gSmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(20, i);
        if (handle != SMBIOS_NOT_FOUND) {
            T20_p = (SMBIOS_MEM_DEV_MAP_ADDR_INFO *)gSmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(handle);
            T20_p->MemoryArrayMapAddrHandle = T19_Handle;
            if (T20_p->MemoryDeviceHandle == SMBIOS_UNKNOWN) {
            // AMI_OVERRIDE_NB0001_START >>>
                T20_Size_KB = (T20_p->EndingAddress + 1) - (T20_p->StartingAddress);
                // AMI_OVERRIDE_NB0001_END <<<
                
                // Looking for Type 17
                for(j = i; j < MAX_HANDLES; j++) {
                    handle = gSmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(17, j);
                    if (handle != SMBIOS_NOT_FOUND) {
                        T17_p = (SMBIOS_MEMORY_DEVICE_INFO *)gSmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(handle);
                        // AMI_OVERRIDE_NB0001_START >>>
                        T17_Size_KB = (UINT32)(T17_p->Size & 0x7FFF);
                        if (T17_Size_KB == 0x7FFF) {                // EIP461425
                          T17_Size_KB = (UINT32)(T17_p->ExtendedSize);  
                          T17_Size_KB = T17_Size_KB * 1024;
                        } else {
                          T17_Size_KB = T17_Size_KB * ((T17_p->Size & 0x8000) ? 1 : 1024);
                        }
                        if (T17_Size_KB == T20_Size_KB) {
                            if(!Lock_T17_Handle_Tmp) {
                                if(T17_Handle_Tmp != T17_p->StructureType.Handle) {
                                    Lock_T17_Handle_Tmp = TRUE;
                                    T17_Handle_Tmp = T17_p->StructureType.Handle;
                                    T20_p->MemoryDeviceHandle = T17_p->StructureType.Handle;
                                }
                            }
                        }
                        // AMI_OVERRIDE_NB0001_END <<<
                    }
                }
            }
            Q_flag++;
        }

        if (Q_flag == 0)
            break;
    }
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
