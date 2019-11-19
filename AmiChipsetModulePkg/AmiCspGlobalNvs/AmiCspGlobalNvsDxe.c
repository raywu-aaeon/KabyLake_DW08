//*************************************************************************
//**                                                                     **
//**   Copyright (c) 1985-2019, American Megatrends International LLC.   **
//**                                                                     **
//**      All rights reserved. Subject to AMI licensing agreement.       **
//**                                                                     **
//*************************************************************************

/** @file AmiCspGlobalNvsDxe.c
    This file contains code for Chipset Global Nvs initialization
    in the DXE stage

*/

// Module specific Includes
#include <Token.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <IndustryStandard/Acpi.h>
#include <Protocol/AcpiTable.h>
#include <AmiCspGlobalNvsAreaProtocol.h>


EFI_ACPI_SDT_PROTOCOL           *gAcpiSdt = NULL;
EFI_ACPI_TABLE_PROTOCOL         *gAcpiTable = NULL;
AMI_CSP_GLOBALNVS_AREA_PROTOCOL gAmiCspGlobalNvsAreaProtocol;
UINTN   DsdtAcpiHandle;


/**
    Get DSDT table pointer and Handle

    @param *DsdtPtr                 - The table points to DSDT table.
    @param *AcpiHandle              - The handle of DSDT table.

    @retval EFI_SUCCESS             - The function completed successfully.
    @retval EFI_INVALID_PARAMETER   - gAcpiSdt or Dsdt is NULL.
    @retval EFI_NOT_FOUND           - DSDT is not found.

**/
EFI_STATUS
EFIAPI
GetAcpiDsdt(
    IN OUT  EFI_ACPI_SDT_HEADER     **Dsdt
    )
{
    UINTN                       Index;
    EFI_ACPI_SDT_HEADER         *Table = NULL;
    EFI_ACPI_TABLE_VERSION      Ver;
    UINTN                       AcpiHandle;
    EFI_STATUS                  Status = EFI_NOT_FOUND;
    
    if (gAcpiSdt == NULL) return EFI_INVALID_PARAMETER;
    if (Dsdt==NULL) return EFI_INVALID_PARAMETER;    
    
    for (Index=0; ; Index++) {
        Status = gAcpiSdt->GetAcpiTable(Index, &Table, &Ver, &AcpiHandle);
        if (Status == EFI_NOT_FOUND) {
            return  Status;
        }
        if (Table->Signature == EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
            *Dsdt = Table;
            DsdtAcpiHandle = AcpiHandle;
            return EFI_SUCCESS;
        }
        #if ACPI_MODULE_VER < 170
        FreePool ((VOID *)Table);
        #endif     
    }
    
    return EFI_SUCCESS;
}

/**
    Update Name Data in  DSDT table

    @param AslSignature             - The signature of Operation Region that we want to update.
    @param Length                   - length of data to be overwritten
    @param Buffer                   - source of data to be written over original aml
    @param DsdtPtr                  - The table points to DSDT table.

    @retval EFI_SUCCESS             - The function completed successfully.
    @retval EFI_NOT_FOUND           - AslSignature is not found in DSDT.

**/
EFI_STATUS
EFIAPI
UpdateNameInDsdtAcpiTable (
    IN UINT32               AslSignature,
    IN UINTN                Length,
    IN VOID                 *Buffer,
    IN EFI_ACPI_SDT_HEADER  *DsdtPtr
    )
{
    UINT8                   *CurrPtr;
    UINT32                  *Signature;
    UINT8                   *DsdtPointer;
    UINT8                   DataSize;
    
    CurrPtr = (UINT8 *) DsdtPtr;
    for (DsdtPointer = CurrPtr;
       DsdtPointer < (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length);
       DsdtPointer++ )
    {
        Signature = (UINT32 *) DsdtPointer;
        if ((*Signature) == AslSignature) {
            if (*(DsdtPointer-1) == AML_NAME_OP) {
                DataSize = *(DsdtPointer+4);
                if ((Length == 1 && DataSize == 0xA) ||
                    (Length == 2 && DataSize == 0xB) ||
                    (Length == 4 && DataSize == 0xC)) {
                    CopyMem (DsdtPointer+5, Buffer, Length);
                    return EFI_SUCCESS;
                } else if (Length == 1 && ((*(UINT8*) Buffer) == 0 || (*(UINT8*) Buffer) == 1) && (DataSize == 0 || DataSize == 1)) {
                    CopyMem (DsdtPointer+4, Buffer, Length);
                    return EFI_SUCCESS;
                }
            }
        }  
    }// end of for loop
    return EFI_NOT_FOUND;
}

/**
    Update Csp Nvs Data to DSDT table

    @param DsdtPtr   - The table points to DSDT table.
    @param *Handle   - The handle of DSDT table.

    @retval VOID

**/
VOID
EFIAPI
UpdateCspNvsDataInDsdtAcpiTable (
    IN EFI_ACPI_SDT_HEADER  *DsdtPtr,
    IN OUT UINTN            *Handle
    )
{
    EFI_ACPI_SDT_HEADER     *Table;
    EFI_STATUS              Status;
    UINT32                  Address;
    UINT16                  Length;
    
    if (gAcpiTable == NULL) {
        DEBUG((DEBUG_ERROR, "UpdateCspNvsDataInDsdtAcpiTable: gAcpiTable is NULL.\n"));
        return;
    }

    Table = AllocateCopyPool (DsdtPtr->Length, DsdtPtr); // create a copy
    if (Table == NULL) {
        DEBUG((DEBUG_ERROR, "UpdateCspNvsDataInDsdtAcpiTable: Fail to create a copy of DSDT.\n"));
        return;
    }
    
    Address = (UINT32) (UINTN)(gAmiCspGlobalNvsAreaProtocol.Area);
    Status  = UpdateNameInDsdtAcpiTable (SIGNATURE_32 ('C','N','V','B'), sizeof (Address), &Address, Table);
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "UpdateCspNvsDataInDsdtAcpiTable: Fail to update CNVB in DSDT.\n"));
        return;
    }
    
    Length  = (UINT16) sizeof (AMI_CSP_NVS_AREA);
    Status  = UpdateNameInDsdtAcpiTable (SIGNATURE_32 ('C','N','V','L'), sizeof (Length), &Length, Table);
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "UpdateCspNvsDataInDsdtAcpiTable: Fail to update CNVL in DSDT.\n"));
        return;
    }
    
    Status = gAcpiTable->UninstallAcpiTable (
                            gAcpiTable,
                            *Handle
                            );
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "UpdateCspNvsDataInDsdtAcpiTable: Fail to uninstall DSDT.\n"));
        return;
    }
    
    *Handle = 0;
    Status = gAcpiTable->InstallAcpiTable (
                            gAcpiTable,
                            Table,
                            Table->Length,
                            Handle
                            );
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "UpdateCspNvsDataInDsdtAcpiTable: Fail to install DSDT.\n"));
        return;
    }
    
    #if ACPI_MODULE_VER < 170
    FreePool ((VOID *)DsdtPtr);
    #endif
}

/**
  This function handles PlatformInit task at the end of DXE

  @param[in]  Event
  @param[in]  *Context
**/
VOID
EFIAPI
CspGlobalNvsInitEndOfDxe (
    IN EFI_EVENT    Event,
    IN VOID         *Context
    )
{
    EFI_STATUS                  Status;
    EFI_ACPI_SDT_HEADER         *DsdtPtr;
    
    Status = gBS->LocateProtocol (&gEfiAcpiSdtProtocolGuid, NULL, (VOID **) &gAcpiSdt);
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "CspGlobalNvsInitEndOfDxe: Fail to locate AcpiSdtProtocol.\n"));
        return;
    }
    
    Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &gAcpiTable);
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "CspGlobalNvsInitEndOfDxe: Fail to locate AcpiTableProtocol.\n"));
        return;
    }
    
    DsdtAcpiHandle = 0;
    Status = GetAcpiDsdt(&DsdtPtr);
    if (!EFI_ERROR(Status)) {
        DEBUG((DEBUG_INFO, "CspGlobalNvsInitEndOfDxe: Found DSDT Table at 0x%08X, Handle is 0x%08X\n", DsdtPtr, DsdtAcpiHandle));
        UpdateCspNvsDataInDsdtAcpiTable (DsdtPtr, &DsdtAcpiHandle);
    }

    gBS->CloseEvent (Event);
}

/**
    This function is the entry point for this DXE. This function
    initializes the chipset Global Nvs.

    @param ImageHandle - Image handle
    @param SystemTable - Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
            time to expire.
**/
EFI_STATUS
EFIAPI
AmiCspGlobalNvsDxeInit (
    IN EFI_HANDLE               ImageHandle,
    IN EFI_SYSTEM_TABLE         *SystemTable
)
{
    EFI_PHYSICAL_ADDRESS        Address;
    UINTN                       Pages;
    EFI_STATUS                  Status;
    EFI_HANDLE                  Handle;
    UINT32                      CspGlobalNvsAreaPtr;
    EFI_EVENT                   EndOfDxeEvent;

    Address = 0xffffffff; // allocate address below 4G.
    Pages   = EFI_SIZE_TO_PAGES (sizeof (AMI_CSP_NVS_AREA));
    Status  = gBS->AllocatePages (
                     AllocateMaxAddress,
                     EfiReservedMemoryType,
                     Pages,
                     &Address
                     );
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "AmiCspGlobalNvsDxeInit: Fail to allocate memory buffer for AMI_CSP_NVS_AREA.\n"));
        return Status;
    }

    gBS->SetMem ((VOID *)Address, sizeof (AMI_CSP_NVS_AREA), 0);
    gAmiCspGlobalNvsAreaProtocol.Area = (VOID *)(UINTN) Address;
    CspGlobalNvsAreaPtr = (UINT32) Address;
    DEBUG((DEBUG_ERROR, "AmiCspGlobalNvsDxeInit: CspGlobalNvsAreaPtr is at 0x%x.\n", CspGlobalNvsAreaPtr));

    Handle = NULL;
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &Handle,
                    &gAmiCspGlobalNvsAreaProtocolGuid,
                    &gAmiCspGlobalNvsAreaProtocol,
                    NULL
                    );
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "AmiCspGlobalNvsDxeInit: Fail to install AmiCspGlobalNvsAreaProtocol.\n"));
        return Status;
    }      

    Status = gRT->SetVariable(
                    AMI_CSP_GLOBALNVS_PTR_VARIABLE,
                    &gAmiCspGlobalNvsPtrGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (CspGlobalNvsAreaPtr),
                    &CspGlobalNvsAreaPtr
                    );
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "AmiCspGlobalNvsDxeInit: Fail to save AmiCspGlobalNvsPtr variable.\n"));
        return Status;
    }        
    
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    CspGlobalNvsInitEndOfDxe,
                    NULL,
                    &gEfiEndOfDxeEventGroupGuid,
                    &EndOfDxeEvent
                    );
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "AmiCspGlobalNvsDxeInit: Fail to create EndOfDxe event.\n"));
        return Status;
    }  

    return Status;
}

