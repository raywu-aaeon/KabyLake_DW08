//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file FitHook.c
    SW SMI hook.

**/
//*************************************************************************

#include <AmiDxeLib.h>
#include <Protocol/SmiFlash.h>
#include <Library/DebugLib.h>
#include "Token.h"

#define FLASH_DEVICE_BASE_ADDRESS       (0xFFFFFFFF-FLASH_SIZE+1)
#define FV_DATA_BLOCK_ADDRESS           (FV_DATA_BASE-FLASH_DEVICE_BASE_ADDRESS)
#define FV_DATA_BLOCK_END               (FV_DATA_BLOCK_ADDRESS+FV_DATA_SIZE)
#define FV_DATA_TABLE_OFFSET            (0xFFFFFFFF-FV_DATA_BASE+1)

/**
    This function is SW SMI hook that sets Flash Block Description
    type for AMI AFU utility. (EIP#58139)

    @param 
        SwSmiNum    - SW SMI value number
        Buffer      - Flash descriptor address

    @retval VOID

**/

VOID AppendFvDataToAFU_UpdateBlockTypeId (
    IN UINT8  SwSmiNum,
    IN UINT64 Buffer )
{
    BLOCK_DESC *BlockDesc;
    UINTN  i;

    // return if SW SMI value is not "Get Flash Info"
    if (SwSmiNum != SMIFLASH_GET_FLASH_INFO)
        return;

    BlockDesc = (BLOCK_DESC*)&((INFO_BLOCK*)Buffer)->Blocks;

    for (i = 0; i < ((INFO_BLOCK*)Buffer)->TotalBlocks; i++) {

    	DEBUG((DEBUG_INFO,"AppendFvDataToAFU_UpdateBlockTypeId: %08X(%08X), Block %08X\n", FV_DATA_BLOCK_ADDRESS, FV_DATA_BLOCK_END, BlockDesc[i].StartAddress));

        if (BlockDesc[i].StartAddress < FV_DATA_BLOCK_ADDRESS)
            continue;

        if (BlockDesc[i].StartAddress >= FV_DATA_BLOCK_END)
            continue;

        DEBUG((DEBUG_INFO,"AppendFvDataToAFU_UpdateBlockTypeId: Found Blocks %08X\n",BlockDesc[i].StartAddress));

        BlockDesc[i].Type = BOOT_BLOCK;
    }
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
