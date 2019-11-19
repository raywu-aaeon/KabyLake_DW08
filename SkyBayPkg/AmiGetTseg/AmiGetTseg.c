#include <Efi.h>
#include <Token.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/SmmAccess2.h>
#include <Library/DebugLib.h>
#include <AmiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>

#define AMI_TSEG_INFO_GUID \
    {0x202f0384, 0xaa82, 0x4a04, 0xae, 0x64, 0x2c, 0x21, 0xf4, 0x61, 0xa5, 0xc}

#define Ami_Header_Version 1

void
TitleString (
    void
)
{
    CHAR16      s[100];

    Swprintf(s, L"Get Tseg Info Utility V0.%d\n\r", Ami_Header_Version);
    gST->ConOut->OutputString(gST->ConOut, s);
    Swprintf(s, L"(c)2015 Copyright American Megatrends Inc\n\r\n\r");
    gST->ConOut->OutputString(gST->ConOut, s);
}

//
// Module entry point
//
EFI_STATUS
EFIAPI
AmiGetTsegEntryPoint (
    IN EFI_HANDLE                     ImageHandle,
    IN EFI_SYSTEM_TABLE               *SystemTable )
{
    EFI_STATUS                Status;
    EFI_SMM_ACCESS2_PROTOCOL  *SmmAccess;
    UINTN                     Size, DataSize, UnusedTsegPageSize;
    CHAR16                    s[50];
    EFI_SMRAM_DESCRIPTOR      *mSmramRanges;
    UINTN                     i, mSmramRangeCount, TsegSize = 0;
    EFI_GUID                  gAmiTsegInfoGuid = AMI_TSEG_INFO_GUID;

    TitleString();

    //
    // Locate SMM Access2 Protocol
    //
    Status = gBS->LocateProtocol (
                 &gEfiSmmAccess2ProtocolGuid,
                 NULL,
                 (VOID **)&SmmAccess
             );
    ASSERT_EFI_ERROR (Status);

    //
    // Get SMRAM range information
    //
    Size = 0;
    Status = SmmAccess->GetCapabilities (SmmAccess, &Size, NULL);
    ASSERT (Status == EFI_BUFFER_TOO_SMALL);

    mSmramRanges = (EFI_SMRAM_DESCRIPTOR *) AllocatePool (Size);
    ASSERT (mSmramRanges != NULL);

    Status = SmmAccess->GetCapabilities (SmmAccess, &Size, mSmramRanges);
    ASSERT_EFI_ERROR (Status);

    mSmramRangeCount = Size / sizeof (EFI_SMRAM_DESCRIPTOR);

    Swprintf(s, L"Total mSmramRangeCount              : %d\n\r",mSmramRangeCount);
    gST->ConOut->OutputString(gST->ConOut, s);

    for ( i=0; i < mSmramRangeCount ; i++ ) {
        Swprintf(s, L"Tseg mSmramRanges[%d]->CpuStart      : 0x%X\n\r", i, mSmramRanges->CpuStart);
        gST->ConOut->OutputString(gST->ConOut, s);
        Swprintf(s, L"Tseg mSmramRanges[%d]->PhysicalSize  : 0x%X\n\r", i, mSmramRanges->PhysicalSize);
        gST->ConOut->OutputString(gST->ConOut, s);
        Swprintf(s, L"Tseg mSmramRanges[%d]->PhysicalStart : 0x%X\n\r", i, mSmramRanges->PhysicalStart);
        gST->ConOut->OutputString(gST->ConOut, s);
        Swprintf(s, L"Tseg mSmramRanges[%d]->RegionState   : 0x%X\n\n\r", i, mSmramRanges->RegionState);
        gST->ConOut->OutputString(gST->ConOut, s);
        TsegSize += mSmramRanges->PhysicalSize;
        mSmramRanges++;
    }

    Swprintf(s, L"Total Tseg Size : 0x%X (%d KB)\n\r",TsegSize, TsegSize/1024);
    gST->ConOut->OutputString(gST->ConOut, s);

    IoWrite8(0xB2, CRB_SWSMI);

    DataSize = sizeof (UINT32);
    UnusedTsegPageSize = 0;
    Status = gRT->GetVariable (
                 L"GetTsegInfo",
                 &gAmiTsegInfoGuid,
                 NULL,
                 &DataSize,
                 &UnusedTsegPageSize );

    Swprintf(s, L"Used  Tseg Size : 0x%X (%d KB)\n\r",TsegSize - EFI_PAGES_TO_SIZE(UnusedTsegPageSize), (TsegSize - EFI_PAGES_TO_SIZE(UnusedTsegPageSize))/1024 );
    gST->ConOut->OutputString(gST->ConOut, s);

    Swprintf(s, L"Free  Tseg Size : 0x%X (%d KB)\n\r",EFI_PAGES_TO_SIZE(UnusedTsegPageSize), EFI_PAGES_TO_SIZE(UnusedTsegPageSize)/1024);
    gST->ConOut->OutputString(gST->ConOut, s);
    return EFI_SUCCESS;
}
