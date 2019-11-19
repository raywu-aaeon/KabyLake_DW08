//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file BiosGuardNvs.c
    BiosGuardNvs for Tool Interface

**/
#include <Token.h>
#include <AmiCspLib.h>
#include <Protocol/CpuGlobalNvsArea.h>
#include <Protocol/AcpiTable.h>
#include <AcpiRes.h>
#include <BiosGuardNvs.h>
#include <Uefi/UefiBaseType.h>
#include <IndustryStandard/AcpiAml.h>
#include <CpuRegs.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <SetupVariable.h>

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
AMI_BIOS_GUARD_NVS_AREA_PROTOCOL      mAmiBiosGuardNvsAreaProtocol;
CPU_GLOBAL_NVS_AREA_PROTOCOL          *mCpuGlobalNvsAreaProtocol = NULL;
EFI_HANDLE                            mImageHandle;
EFI_GUID                              gAmiBiosGuardNvsAreaProtocolGuid = AMI_BIOS_GUARD_NVS_AREA_PROTOCOL_GUID;

VOID
InstallBiosGuardNvs (
    VOID
)
{
    EFI_STATUS                        Status;
    UINTN                             Pages;
    EFI_PHYSICAL_ADDRESS              Address;
    EFI_ACPI_DESCRIPTION_HEADER       *DsdtTable;
    EFI_PHYSICAL_ADDRESS              DsdtAddr;
    UINT32                            Length;
    UINT8                             *ptr;
    AMI_BIOS_GUARD_RECOVERY           *ApRecoveryAddress;
    ASL_OBJ_INFO                      ObjInfo;
    UINT32                            BiosGuardRecoveryAddress;    
    UINTN                             BiosGuardRecoveryAddressVarSize = sizeof(UINT32);
    EFI_GUID                          BiosGuardRecoveryAddressGuid = BIOS_GUARD_RECOVERY_ADDRESS_GUID;
    UINTN                             VariableSize;
    SETUP_DATA                        SetupData;

    Pages = EFI_SIZE_TO_PAGES (sizeof (AMI_BIOS_GUARD_NVS_AREA_PROTOCOL));

    Address = 0xffffffff; // allocate address below 4G.

    Status  = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiACPIMemoryNVS,
                  Pages,
                  &Address
              );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return;

    mAmiBiosGuardNvsAreaProtocol.Area = (AMI_BIOS_GUARD_NVS_AREA *)(UINTN) Address;

    gBS->SetMem (mAmiBiosGuardNvsAreaProtocol.Area, sizeof (AMI_BIOS_GUARD_NVS_AREA), 0);

    //
    // mAmiBiosGuardNvsAreaProtocol.Area has the address of BGNV in ACPI NVS area
    // update this as BGNV opRegion field of BGNV area in DSDT.
    //
    Status = LibGetDsdt(&DsdtAddr, EFI_ACPI_TABLE_VERSION_ALL);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "Fail to Get DSDT - returned %r\n", Status));
        ASSERT_EFI_ERROR(Status);
        return;
    }
    DsdtTable = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)DsdtAddr;

    if (DsdtTable) {
        Length = DsdtTable->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER);
        ptr = (UINT8*)((UINTN)DsdtTable + sizeof(EFI_ACPI_DESCRIPTION_HEADER));
        Status = GetAslObj(ptr, Length, "ABNV", otOpReg, &ObjInfo);
        ASSERT_EFI_ERROR (Status);

        ptr = (UINT8*)ObjInfo.DataStart;
        *(UINT32*)(ptr + 2) = (UINT32)(UINTN)mAmiBiosGuardNvsAreaProtocol.Area;
        *(UINT16*)(ptr + 2 + 5) = sizeof (AMI_BIOS_GUARD_NVS_AREA);
        DEBUG((DEBUG_INFO, "ACPI Bios Guard NVS Ptr=0x%X, Length=0x%X\n", (UINT32)(UINTN) mAmiBiosGuardNvsAreaProtocol.Area, sizeof (AMI_BIOS_GUARD_NVS_AREA)));
    }

    Status = gBS->AllocatePool (EfiACPIMemoryNVS, sizeof (AMI_BIOS_GUARD_RECOVERY), &ApRecoveryAddress);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return;

    ApRecoveryAddress->Signature = SIGNATURE_64('_', 'A', 'M', 'I', 'B', 'S', 'G', 'D');
    ApRecoveryAddress->Version = 0;
    ApRecoveryAddress->ReoveryMode = 0;
    ApRecoveryAddress->ReoveryAddress = 0;
    
    //
    // Locate CPU Global NVS
    //
    Status = gBS->LocateProtocol (&gCpuGlobalNvsAreaProtocolGuid, NULL, (VOID **) &mCpuGlobalNvsAreaProtocol);
    ASSERT_EFI_ERROR (Status);

    VariableSize = sizeof (SETUP_DATA);
    Status = gRT->GetVariable (
                      L"Setup",
                      &gSetupVariableGuid,
                      NULL,
                      &VariableSize,
                      &SetupData
                      );
    ASSERT_EFI_ERROR (Status);
    
    mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardMemAddress = mCpuGlobalNvsAreaProtocol->Area->BiosGuardMemAddress;
    mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardMemSize = mCpuGlobalNvsAreaProtocol->Area->BiosGuardMemSize;
    mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardIoTrapAddress = mCpuGlobalNvsAreaProtocol->Area->BiosGuardIoTrapAddress;
    mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardIoTrapLength = mCpuGlobalNvsAreaProtocol->Area->BiosGuardIoTrapLength;
    mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardRecoveryAddress = (UINT32)ApRecoveryAddress;
    mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardRecoverySize = sizeof(AMI_BIOS_GUARD_NVS_AREA);
    mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardFlashWearOutProtection = SetupData.FlashWearOutProtection;
    mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardBgtSupportVersion = BIOS_GUARD_BGT_VERSION;    

    DEBUG((DEBUG_INFO, "[BiosGuardNvs.c] mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardMemAddress = %X\n", mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardMemAddress));
    DEBUG((DEBUG_INFO, "[BiosGuardNvs.c] mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardMemSize = %X\n", mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardMemSize));
    DEBUG((DEBUG_INFO, "[BiosGuardNvs.c] mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardIoTrapAddress = %X\n", mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardIoTrapAddress));
    DEBUG((DEBUG_INFO, "[BiosGuardNvs.c] mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardIoTrapLength = %X\n", mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardIoTrapLength));
    DEBUG((DEBUG_INFO, "[BiosGuardNvs.c] mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardRecoveryAddress = %X\n", mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardRecoveryAddress));
    DEBUG((DEBUG_INFO, "[BiosGuardNvs.c] mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardRecoverySize = %X\n", mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardRecoverySize));    
    DEBUG((DEBUG_INFO, "[BiosGuardNvs.c] mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardFlashWearOutProtection = %X\n", mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardFlashWearOutProtection));    
    DEBUG((DEBUG_INFO, "[BiosGuardNvs.c] mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardBgtSupportVersion = %X\n", mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardBgtSupportVersion));    

    Status = gBS->InstallMultipleProtocolInterfaces (
                 &mImageHandle,
                 &gAmiBiosGuardNvsAreaProtocolGuid,
                 &mAmiBiosGuardNvsAreaProtocol,
                 NULL);

    BiosGuardRecoveryAddress = mAmiBiosGuardNvsAreaProtocol.Area->AmiBiosGuardRecoveryAddress;
    Status = gRT->SetVariable(L"BiosGuardRecoveryAddressVariable",
                              &BiosGuardRecoveryAddressGuid,
                              (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                              BiosGuardRecoveryAddressVarSize,
                              &BiosGuardRecoveryAddress);

    ASSERT_EFI_ERROR(Status);

    return;
}

/**
    This function can initialize AMI BiosGuard tool interface

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/

VOID 
EFIAPI
InitBiosGuardNvsEvent (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{ 
	InstallBiosGuardNvs();
}

EFI_STATUS
EFIAPI
InstallBiosGuardNvsEntry (
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
)
{
	EFI_STATUS		Status;
	EFI_EVENT       EndOfDxeEvent;
	
    InitAmiLib(ImageHandle,SystemTable);

    mImageHandle = ImageHandle;

    DEBUG((DEBUG_INFO, "[BiosGuardNvs.c] InstallBiosGuardNvs Entry\n"));
    
    if ( !((AsmReadMsr64 (MSR_PLATFORM_INFO) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) &&
            (AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) ) {
        DEBUG ((DEBUG_INFO, "BIOS Guard is disabled or not supported \n"));
        return EFI_SUCCESS;
    }

    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    InitBiosGuardNvsEvent,
                    NULL,
                    &gEfiEndOfDxeEventGroupGuid,
                    &EndOfDxeEvent
                    );
    ASSERT_EFI_ERROR(Status);    

    DEBUG((DEBUG_INFO, "[BiosGuardNvs.c] InstallBiosGuardNvs End\n"));
    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
