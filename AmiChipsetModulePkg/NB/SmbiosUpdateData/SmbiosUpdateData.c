//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#include <AmiDxeLib.h>
#include <Protocol/AmiSmbios.h>
#include <Library/DebugLib.h>
#include "SmbiosUpdateDataProtocol.h"

AMI_SMBIOS_PROTOCOL	*gAmiSmbiosProtocol;

EFI_SMBIOS_UPDATE_DATA_PROTOCOL SmbiosUpdateDataProtocol = {
    SMBIOS_GetFreeHandle,
    SMBIOS_FindStructureByType,
    SMBIOS_GetStructureBase,
    SMBIOS_DeleteStructure,
    SMBIOS_CopyStructure,
    SMBIOS_InsertStructure
};

/**
    Returns the length of the structure pointed by BufferStart
    in bytes

    @param BufferStart 

    @retval Structure Size

**/
UINT16
GetStructureLength(
	IN UINT8     *BufferStart
)
{
	UINT8	*BufferEnd = BufferStart;

	BufferEnd += ((SMBIOS_STRUCTURE_HEADER*)BufferStart)->Length;
	while (*(UINT16*)BufferEnd != 0) {
		BufferEnd++;
	}

	return (UINT16)(BufferEnd + 2 - BufferStart);   // +2 for double zero terminator
}

/**
    Find structure type starting from memory location pointed by
    Buffer

    @param Buffer 
    @param StructureFoundPtr 
    @param SearchType 
    @param Instance 

    @retval If SearchType is found:
        UINT8   **Buffer - Points to the next structure
        UINT8   **StructureFoundPtr - Points to the structure
        that was found
        If SearchType is not found:
        UINT8   **Buffer - No change
        UINT8   **StructureFoundPtr = NULL

**/
BOOLEAN
FindStructureByType(
  IN OUT  UINT8    **Buffer,
  IN OUT  UINT8    **StructureFoundPtr,
  IN      UINT8    SearchType,
  IN      UINT8    Instance             // 1-based
)
{
  UINT8     *BufferPtr = *Buffer;
  BOOLEAN   FindStatus = FALSE;

  *StructureFoundPtr = NULL;
  
  while (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Type != 127) {
    if (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Type == SearchType) {
      // If this instance, set the find status flag and update the Buffer pointer
      if (--Instance == 0) {
        FindStatus = TRUE;
        *StructureFoundPtr = BufferPtr;
#if 0        
        DEBUG ((DEBUG_INFO, "FindStructureByType - StructureFoundPtr = %x\n", BufferPtr));
        {
        	UINT8	*TempPtr = BufferPtr;
            DEBUG ((DEBUG_INFO, "Structure Begin\n"));
            while (*(UINT16*)TempPtr != 0) {
                DEBUG ((DEBUG_INFO, "Value = %x\n", *TempPtr));
            	TempPtr++;
            }
            DEBUG ((DEBUG_INFO, "Structure End\n"));
        }
        DEBUG ((DEBUG_INFO, "FindStructureByType - GetStructureLength = %x\n", GetStructureLength(BufferPtr)));
#endif     
        *Buffer = BufferPtr + GetStructureLength(BufferPtr);
        break;
      }
    }
    BufferPtr += GetStructureLength(BufferPtr);
  }
  if ((FindStatus == FALSE) && (SearchType == 127)) {
    FindStatus = TRUE;
    *StructureFoundPtr = BufferPtr;
    *Buffer = BufferPtr + GetStructureLength(BufferPtr);
  }
  return FindStatus;
}

/**
    Find structure handle starting from memory location pointed
    by Buffer

    @param Buffer 
    @param Handle 

    @retval If SearchType is found:
        UINT8   **Buffer - Points to the structure that was found

**/
BOOLEAN
FindStructureByHandle(
    IN OUT  UINT8     **Buffer,
    IN      UINT16    Handle
)
{
	while (((SMBIOS_STRUCTURE_HEADER*)*Buffer)->Handle != Handle) {
		if (((SMBIOS_STRUCTURE_HEADER*)*Buffer)->Type == 127) {
			return FALSE;
		}
		*Buffer += GetStructureLength(*Buffer);
	}

	return TRUE;
}

/**
    Searches available handle of Smbios Data Table

    @param VOID

    @retval UINT16 Handle or -1(if not found)

**/
UINT16
SMBIOS_GetFreeHandle(
)
{
	return gAmiSmbiosProtocol->SmbiosGetFreeHandle();
}

/**
    Searches handle of Smbios Table by Type

    @param Type 
    @param Index 

    @retval UINT16 Handle or -1(if not found)

**/
UINT16
SMBIOS_FindStructureByType(
    IN  UINT8	Type,
    IN  UINT8   Index
)
{
	UINT8       *StructurePtr = NULL;
	UINT8		*SmbiosEntryPointTablePtr;
    UINT8       *SmbiosDataTableAddr;


    // Get Smbios Data Table Address
    SmbiosEntryPointTablePtr = gAmiSmbiosProtocol->SmbiosGetTableEntryPoint();
    SmbiosDataTableAddr = (UINT8*)(UINTN)((SMBIOS_TABLE_ENTRY_POINT*)SmbiosEntryPointTablePtr)->TableAddress;
    //DEBUG ((DEBUG_INFO, "SMBIOS_FindStructureByType - SmbiosDataTableAddr = %x\n", SmbiosDataTableAddr));

    if (SmbiosDataTableAddr == NULL) {
        gAmiSmbiosProtocol->SmbiosUpdateHeader();
        SmbiosDataTableAddr = (UINT8*)(UINTN)((SMBIOS_TABLE_ENTRY_POINT*)SmbiosEntryPointTablePtr)->TableAddress;
    }
    
    // Return Handle of found structure
    if (FindStructureByType(&SmbiosDataTableAddr, &StructurePtr, Type, Index)) {
#if 0        
        DEBUG ((DEBUG_INFO, "SMBIOS_FindStructureByType - SmbiosDataTableAddr = %x\n", SmbiosDataTableAddr));
        DEBUG ((DEBUG_INFO, "SMBIOS_FindStructureByType - StructurePtr = %x\n", StructurePtr));
        DEBUG ((DEBUG_INFO, "SMBIOS_FindStructureByType - Structure found, Type = %x\n", ((SMBIOS_STRUCTURE_HEADER*)StructurePtr)->Type));
        DEBUG ((DEBUG_INFO, "SMBIOS_FindStructureByType - Structure found, Length = %x\n", ((SMBIOS_STRUCTURE_HEADER*)StructurePtr)->Length));
        DEBUG ((DEBUG_INFO, "SMBIOS_FindStructureByType - Structure found, Handle = %x\n", ((SMBIOS_STRUCTURE_HEADER*)StructurePtr)->Handle));
#endif        
	    return ((SMBIOS_STRUCTURE_HEADER*)StructurePtr)->Handle;
    }
    else {
        //DEBUG ((DEBUG_INFO, "SMBIOS_FindStructureByType - Structure not found \n"));
        return (UINT16)-1;
    }
}

/**
    Get structure address by Handle

    @param Handle 

    @retval UINT8* or NULL - Points to the structure that was found

**/
UINT8*
SMBIOS_GetStructureBase(
    IN  UINT16	Handle
)
{
	UINT8	*FoundStructurePtr;
	UINT8	*SmbiosEntryPointTablePtr;
    UINT8   *SmbiosDataTableAddr;

    // Get Smbios Data Table Address
    SmbiosEntryPointTablePtr = gAmiSmbiosProtocol->SmbiosGetTableEntryPoint();
    SmbiosDataTableAddr = (UINT8*)(UINTN)((SMBIOS_TABLE_ENTRY_POINT*)SmbiosEntryPointTablePtr)->TableAddress;
    FoundStructurePtr = SmbiosDataTableAddr;

    if (FindStructureByHandle(&FoundStructurePtr, Handle)) {
        return	FoundStructurePtr;
    }
    else {
        return	NULL;
    }
}

/**
    SMBIOSUpdateData protocol - Delete SMBIOS structure by handle

    @param Handle 

    @retval EFI_STATUS

**/
EFI_STATUS
SMBIOS_DeleteStructure(
    IN  UINT16	Handle
)
{
    return gAmiSmbiosProtocol->SmbiosDeleteStructure(Handle);
}

/**
    SMBIOSUpdateData protocol - Copy SMBIOS structure by Handle
    to a new allocated memory

    @param pSmbiosTableEntryPoint 
    @param Handle 

    @retval UINT8 * or NULL - Points to the new structure address

**/
UINT8 *
SMBIOS_CopyStructure(
    IN  VOID                      *pSmbiosTableEntryPoint,
    IN  UINT16                    Handle
)
{
    EFI_STATUS		FoundStatus;

    UINT16        	BufferSize;
	UINT8         	*BufferPtrTemp;
	UINT8         	*FoundStructurePtr;
	UINT8		  	*SmbiosEntryPointTablePtr;
    UINT8         	*SmbiosDataTableAddr;

    // Get Smbios Data Table Address
    SmbiosEntryPointTablePtr = gAmiSmbiosProtocol->SmbiosGetTableEntryPoint();
    SmbiosDataTableAddr = (UINT8*)(UINTN)((SMBIOS_TABLE_ENTRY_POINT*)SmbiosEntryPointTablePtr)->TableAddress;
    FoundStructurePtr = SmbiosDataTableAddr;

    // Check Handle of Structure if exit
    FoundStatus = FindStructureByHandle(&FoundStructurePtr, Handle);

	if (FoundStatus){
    	BufferSize = GetStructureLength(FoundStructurePtr);

        // Allocate Memory for structure copy
        pBS->AllocatePool(EfiBootServicesData, BufferSize, (VOID **)&BufferPtrTemp);

        // Copy Structure to memory
		pBS->CopyMem(BufferPtrTemp, FoundStructurePtr, BufferSize);

        return BufferPtrTemp;

	} else {
	    return  NULL;
    }

}

/**
    Insert SMBIOS structure

    @param pSmbiosTableEntryPoint 
    @param SourceStructurePtr 
    @param BufferSize 

    @retval EFI_STATUS

**/
EFI_STATUS
SMBIOS_InsertStructure(
    IN  VOID                      *pSmbiosTableEntryPoint,
    IN  UINT8                     *StructurePtrTemp,
    IN  UINT16                    BufferSize
)
{
    UINT16		Handle = 0;

    Handle = ((SMBIOS_STRUCTURE_HEADER*)StructurePtrTemp)->Handle;
    return gAmiSmbiosProtocol->SmbiosAddStrucByHandle(Handle, StructurePtrTemp, BufferSize);
}

/**
    SMBIOSUpdateData protocol - Insert SMBIOS structure

    @param pSmbiosTableEntryPoint 
    @param SourceStructurePtr 
    @param BufferSize 

    @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
SmbiosUpdateDataDriverEntryPoint(
	IN EFI_HANDLE           ImageHandle,
	IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS Status;

	InitAmiLib(ImageHandle,SystemTable);

    Status = pBS->LocateProtocol(&gAmiSmbiosProtocolGuid, NULL, (VOID**)&gAmiSmbiosProtocol);
    ASSERT_EFI_ERROR(Status);

    Status = pBS->InstallProtocolInterface(&ImageHandle,
	                                       &gSmbiosUpdateDataProtocolGuid,
	                                       EFI_NATIVE_INTERFACE,
	                                       &SmbiosUpdateDataProtocol);

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
