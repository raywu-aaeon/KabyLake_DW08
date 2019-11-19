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

#ifndef __SMBIOS_UPDATE_DATA_PROTOCOL__H__
#define __SMBIOS_UPDATE_DATA_PROTOCOL__H__

#ifdef __cplusplus
extern "C" {
#endif

extern EFI_GUID gSmbiosUpdateDataProtocolGuid;

typedef struct _EFI_SMBIOS_UPDATE_DATA_PROTOCOL EFI_SMBIOS_UPDATE_DATA_PROTOCOL;

typedef UINT16 (EFIAPI *EFI_SMBIOS_UPDATE_DATA_GET_FREE_HANDLE) (
);

typedef UINT16 (EFIAPI *EFI_SMBIOS_UPDATE_DATA_FIND_STRUCTURE_BY_TYPE) (
    IN  UINT8                     Type,
    IN  UINT8                     Index
);

typedef UINT8 * (EFIAPI *EFI_SMBIOS_UPDATE_DATA_GET_STRUCTURE_BASE) (
    IN  UINT16                    Handle
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_UPDATE_DATA_DELETE_STRUCTURE) (
    IN  UINT16                    Handle
);

typedef UINT8 * (EFIAPI *EFI_SMBIOS_UPDATE_DATA_COPY_STRUCTURE) (
    IN  VOID                      *pSmbiosTableEntryPoint,
    IN  UINT16                    Handle
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_UPDATE_DATA_INSERT_STRUCTURE) (
    IN  VOID                      *pSmbiosTableEntryPoint,
    IN  UINT8                     *StructurePtrTemp,
    IN  UINT16                    BufferSize
);

struct _EFI_SMBIOS_UPDATE_DATA_PROTOCOL {
    EFI_SMBIOS_UPDATE_DATA_GET_FREE_HANDLE         SMBIOS_GetFreeHandle;
    EFI_SMBIOS_UPDATE_DATA_FIND_STRUCTURE_BY_TYPE  SMBIOS_FindStructureByType;
    EFI_SMBIOS_UPDATE_DATA_GET_STRUCTURE_BASE      SMBIOS_GetStructureBase;
    EFI_SMBIOS_UPDATE_DATA_DELETE_STRUCTURE        SMBIOS_DeleteStructure;
    EFI_SMBIOS_UPDATE_DATA_COPY_STRUCTURE          SMBIOS_CopyStructure;
    EFI_SMBIOS_UPDATE_DATA_INSERT_STRUCTURE        SMBIOS_InsertStructure;
};

UINT16
SMBIOS_GetFreeHandle(
);

UINT16
SMBIOS_FindStructureByType(
    IN  UINT8                     Type,
    IN  UINT8                     Index
);

UINT8 *
SMBIOS_GetStructureBase(
    IN  UINT16                    Handle
);

EFI_STATUS
SMBIOS_DeleteStructure(
    IN  UINT16                    Handle
);

UINT8 *
SMBIOS_CopyStructure(
    IN  VOID                      *pSmbiosTableEntryPoint,
    IN  UINT16                    Handle
);

EFI_STATUS
SMBIOS_InsertStructure(
    IN  VOID                      *pSmbiosTableEntryPoint,
    IN  UINT8                     *StructurePtrTemp,
    IN  UINT16                    BufferSize
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
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
