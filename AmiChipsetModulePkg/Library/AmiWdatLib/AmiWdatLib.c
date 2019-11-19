//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AmiWdatLib.c
    Library Class for Wdat routines.

**/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <AmiDxeLib.h>
#include <Wdat2/Wdat.h>
#include <Token.h>
#include <Library/AmiWdatLib.h>
#include <IndustryStandard/Acpi.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)
/**
  Definition for PCR address
  The PCR address is used to the PCR MMIO programming
**/
#define PCH_PCR_ADDRESS(Pid, Offset)    (0xFD000000 | ((UINT8)(Pid) << 16) | (UINT16)(Offset))

#define TCO_RLD_OFFSET      0x0
#define TCO_TMR_OFFSET      0x12
#define TCO1_CNT_OFFSET     0x8
#define TCO2_CNT_OFFSET     0xA
#define TCO2_STS_OFFSET     0x6
#define GCS_ADDR            PCH_PCR_ADDRESS(0xC6, 0x0C) // R_PCH_PCR_SMBUS_GC
#define OC_WDT_BASE_ADDRESS ACPI_BASE_ADDRESS + 0x54
// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)
///WDAT Header
///                 {WatchdogHeaderLength, PciSegment, PciBusNumber, PciDeviceNumber, PciFuncitonNumber, Reserved1[3], TimerPeriod, MaximumCount, MinimumCount, WatchdogFlags, Reserved2[3], NumberWactchdogInstructionEntries};
WDAT_HDR	WdatHdr = {0, SB_WDAT_PCI_SEG, SB_WDAT_PCI_BUS, SB_WDAT_PCI_DEV, SB_WDAT_PCI_FUN, 0,0,0, SB_WDAT_TIMER_PERIOD, SB_WDAT_MAX_COUNT, SB_WDAT_MIN_COUNT, SB_WDAT_FLAG, 0,0,0, 0};

/// Watchdog Action Instruction Table
///      {WatchdogAction                    ,InstructionFlags                       ,Reserved1  ,RegisterRegion.AddrSpcID,RegisterRegion.RegBitWidth,RegisterRegion.RegBitOffs,RegisterRegion.AccessSize,RegisterRegion.Address, Value, Mask };
/// WatchdogAction: Please refer to Watchdog Actions (WDA) Value defined in Wdat.h.
/// InstructionFlags: Please refer to Watchdog Instruction(WDI) Flags Value defined in Wdat.h.
/// RegisterRegion.AddrSpcID: Please refer to Generic Address Space Address IDs defined in Acpi50.h.
#if defined INTEL_WDT_SWITCH && INTEL_WDT_SWITCH
WDAT_INST_ENTRY NewActionTable[] = {
        {WDA_RESET                         ,WDI_WRITE_COUNTDOWN                    ,0          ,GAS_SYS_IO ,16         ,0          ,2          ,TCO_BASE_ADDRESS    ,0x00   ,0x3ff  },
        {WDA_QUERY_CURRENT_COUNTDOWN_PERIOD,WDI_READ_COUNTDOWN                     ,0          ,GAS_SYS_IO ,16         ,0          ,2          ,TCO_BASE_ADDRESS    ,0x00   ,0x3ff  },
        {WDA_QUERY_COUNTDOWN_PERIOD        ,WDI_READ_COUNTDOWN                     ,0          ,GAS_SYS_IO ,16         ,0          ,2          ,TCO_BASE_ADDRESS + TCO_TMR_OFFSET   ,0x00   ,0x3ff  },
        {WDA_SET_COUNTDOWN_PERIOD          ,WDI_WRITE_COUNTDOWN                    ,0          ,GAS_SYS_IO ,16         ,0          ,2          ,TCO_BASE_ADDRESS + TCO_TMR_OFFSET    ,0x00   ,0x3ff  },
        {WDA_QUERY_RUNNING_STATE           ,WDI_READ_VALUE                         ,0          ,GAS_SYS_IO ,8          ,0         ,1          ,TCO_BASE_ADDRESS + TCO1_CNT_OFFSET+1  ,0x00    ,0x08    },
        {WDA_SET_RUNNING_STATE             ,WDI_WRITE_VALUE|WDI_PRESERVE_REGISTER  ,0          ,GAS_SYS_IO ,8          ,0         ,1          ,TCO_BASE_ADDRESS + TCO1_CNT_OFFSET+1  ,0x00    ,0x08    },
        {WDA_QUERY_STOPPED_STATE           ,WDI_READ_VALUE                         ,0          ,GAS_SYS_IO ,8          ,0         ,1          ,TCO_BASE_ADDRESS + TCO1_CNT_OFFSET+1  ,0x08    ,0x08    },
        {WDA_SET_STOPPED_STATE             ,WDI_WRITE_VALUE|WDI_PRESERVE_REGISTER  ,0          ,GAS_SYS_IO ,8          ,0         ,1          ,TCO_BASE_ADDRESS + TCO1_CNT_OFFSET+1  ,0x08    ,0x08    },
        {WDA_QUERY_REBOOT                  ,WDI_READ_VALUE                         ,0          ,GAS_SYS_MEM,32          ,0          ,3          ,GCS_ADDR      ,0x0    ,0x2    },
        {WDA_SET_REBOOT                    ,WDI_WRITE_VALUE|WDI_PRESERVE_REGISTER  ,0          ,GAS_SYS_MEM,32          ,0          ,3          ,GCS_ADDR      ,0x0    ,0x2    },
        {WDA_QUERY_SHUTDOWN                ,WDI_READ_VALUE                         ,0          ,GAS_SYS_IO ,8          ,0          ,1          ,TCO_BASE_ADDRESS + TCO2_CNT_OFFSET  ,0x10    ,0x30   },
        {WDA_SET_SHUTDOWN                  ,WDI_WRITE_VALUE|WDI_PRESERVE_REGISTER  ,0          ,GAS_SYS_IO ,8          ,0          ,1          ,TCO_BASE_ADDRESS + TCO2_CNT_OFFSET ,0x10    ,0x30   },
        {WDA_QUERY_WATCHDOG_STATUS         ,WDI_READ_VALUE                         ,0          ,GAS_SYS_IO ,8          ,0          ,1          ,TCO_BASE_ADDRESS + TCO2_STS_OFFSET  ,0x02    ,0x02    },
        {WDA_SET_WATCHDOG_STATUS           ,WDI_WRITE_VALUE                        ,0          ,GAS_SYS_IO ,8          ,0          ,1          ,TCO_BASE_ADDRESS + TCO2_STS_OFFSET  ,0x02    ,0x02    }};    

#else
WDAT_INST_ENTRY NewActionTable[] = {
         {WDA_RESET                         ,WDI_WRITE_VALUE|WDI_PRESERVE_REGISTER  ,0          ,GAS_SYS_IO ,8         ,0          ,1          ,OC_WDT_BASE_ADDRESS + 0x03    ,BIT7   ,BIT7 },
         {WDA_QUERY_CURRENT_COUNTDOWN_PERIOD,WDI_READ_COUNTDOWN                     ,0          ,GAS_SYS_IO ,16         ,0          ,2         ,OC_WDT_BASE_ADDRESS + 0x00   ,0x0    ,0x3ff  },
         {WDA_QUERY_COUNTDOWN_PERIOD        ,WDI_READ_COUNTDOWN                     ,0          ,GAS_SYS_IO ,16         ,0          ,2         ,OC_WDT_BASE_ADDRESS + 0x00   ,0x0    ,0x3ff  },
         {WDA_SET_COUNTDOWN_PERIOD          ,WDI_WRITE_COUNTDOWN|WDI_PRESERVE_REGISTER ,0       ,GAS_SYS_IO ,16         ,0          ,2         ,OC_WDT_BASE_ADDRESS + 0x00   ,0x0   ,0x3ff  },
         {WDA_QUERY_RUNNING_STATE           ,WDI_READ_VALUE                         ,0          ,GAS_SYS_IO ,8          ,0         ,1          ,OC_WDT_BASE_ADDRESS + 0x01   ,0xe0    ,0xe0    },
         {WDA_SET_RUNNING_STATE             ,WDI_WRITE_VALUE|WDI_PRESERVE_REGISTER  ,0          ,GAS_SYS_IO ,8          ,0         ,1          ,OC_WDT_BASE_ADDRESS + 0x01   ,0xe0    ,0xe0    },
         {WDA_QUERY_STOPPED_STATE           ,WDI_READ_VALUE                         ,0          ,GAS_SYS_IO ,8          ,0         ,1          ,OC_WDT_BASE_ADDRESS + 0x01   ,0x0    ,0xe0    },
         {WDA_SET_STOPPED_STATE             ,WDI_WRITE_VALUE|WDI_PRESERVE_REGISTER  ,0          ,GAS_SYS_IO ,8          ,0         ,1          ,OC_WDT_BASE_ADDRESS + 0x01   ,0x0    ,0xe0    },
         {WDA_QUERY_REBOOT                  ,WDI_READ_VALUE                         ,0          ,GAS_SYS_MEM,32          ,0          ,3        ,GCS_ADDR   ,0x0    ,0x2    },
         {WDA_SET_REBOOT                    ,WDI_WRITE_VALUE|WDI_PRESERVE_REGISTER  ,0          ,GAS_SYS_MEM,32          ,0          ,3        ,GCS_ADDR   ,0x0    ,0x2    },
         {WDA_QUERY_SHUTDOWN                ,WDI_READ_VALUE                         ,0          ,GAS_SYS_IO ,16          ,0          ,2        ,TCO_BASE_ADDRESS + TCO2_CNT_OFFSET   ,0x10    ,0x30    },
         {WDA_SET_SHUTDOWN                  ,WDI_WRITE_VALUE|WDI_PRESERVE_REGISTER  ,0          ,GAS_SYS_IO ,16          ,0          ,2        ,TCO_BASE_ADDRESS + TCO2_CNT_OFFSET   ,0x10    ,0x30    },
         {WDA_QUERY_WATCHDOG_STATUS         ,WDI_READ_VALUE                         ,0          ,GAS_SYS_IO ,8          ,0          ,1         ,OC_WDT_BASE_ADDRESS + 0x02  ,BIT7    ,BIT7    },
         {WDA_SET_WATCHDOG_STATUS           ,WDI_WRITE_VALUE|WDI_PRESERVE_REGISTER  ,0          ,GAS_SYS_IO ,8          ,0          ,1         ,OC_WDT_BASE_ADDRESS + 0x02   ,BIT7    ,BIT7    }}; 
#endif
// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)
///--------------------------------------------------------------------------

/**
    Create ActionTable, retrieve data and ActionTableLen
    
    @retval ActionTable A pointer to ActionTable data.  
    
**/
WDAT_INST_ENTRY*
CreateActionTableAndRetrieveIt(
		OUT UINT32			*ActionTableLen)
{
	EFI_STATUS	Status;
	WDAT_INST_ENTRY	*ActionTable;

	*ActionTableLen = sizeof(NewActionTable) / sizeof(WDAT_INST_ENTRY);
	
	Status = pBS->AllocatePool(EfiBootServicesData, sizeof(NewActionTable), (VOID **)&ActionTable);
	ASSERT(Status == EFI_SUCCESS);
	
	MemCpy(ActionTable, NewActionTable, sizeof(NewActionTable));
	
	return ActionTable;
}

/**
    Override the default Wdat Header and Action instruction table
    
    @retval EFI_SUCCESS Always returns EFI_SUCCESS.  
    
**/
EFI_STATUS
WdatHeaderAndActionTableOverride(
		IN WDA_TABLE 		*pWdatTable,
		IN WDAT_INST_ENTRY	*ActionTable,
		IN UINT32			*ActionTableLen)
{
	UINT64		WdatInstTableLen;
	UINT8		*ActInsEtnry = (UINT8*)pWdatTable + sizeof(ACPI_HDR) + sizeof(WDAT_HDR);
	
	WdatInstTableLen = *ActionTableLen * sizeof(WDAT_INST_ENTRY);
	WdatHdr.WatchdogHeaderLength = sizeof(WDAT_HDR);
	WdatHdr.NumberWactchdogInstructionEntries = *ActionTableLen;
	MemCpy(&pWdatTable->WatchdogHeader, &WdatHdr, sizeof(WDAT_HDR));
	MemCpy(ActInsEtnry, ActionTable,(UINTN)WdatInstTableLen);

	return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
